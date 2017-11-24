// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Copyright (c) 2017 Illumina, Inc.
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * \brief Test paragraph aligner
 *
 * \file test_paragraph.cpp
 * \author Peter Krusche
 * \email pkrusche@illumina.com
 *
 */

#include "gtest/gtest.h"

#include <sstream>
#include <string>

#include "common.hh"

#include "common/ReadExtraction.hh"
#include "paragraph/Disambiguation.hh"
#include "paragraph/Parameters.hh"

// Error.hh always needs to go last
#include "common/Error.hh"

using namespace paragraph;

TEST(Paragraph, AlignsPGHetIns)
{
    auto logger = LOG();

    std::string bam_path = g_testenv->getBasePath() + "/../share/test-data/paragraph/pg-het-ins/na12878.bam";
    std::string graph_spec_path = g_testenv->getBasePath() + "/../share/test-data/paragraph/pg-het-ins/pg-het-ins.json";

    std::string reference_path = g_testenv->getHG38Path();
    if (reference_path.empty())
    {
        logger->warn("Warning: cannot do round-trip testing for paragraph without hg38 reference file -- please "
                     "specify a location using the HG38 environment variable.");
        return;
    }

    Parameters parameters(10000, 3, 0.01f, 0.8f, Parameters::output_options::ALL, true);

    parameters.load(bam_path, graph_spec_path, reference_path);

    common::ReadBuffer all_reads;
    common::extractReads(
        parameters.bam_path(), reference_path, parameters.target_regions(), (int)parameters.max_reads(), all_reads);
    const auto output = alignAndDisambiguate(parameters, all_reads);

    ASSERT_EQ(output["read_counts_by_edge"]["total"].asUInt64(), 29ull);
    ASSERT_EQ(output["read_counts_by_edge"]["total:FWD"].asUInt64(), 14ull);
    ASSERT_EQ(output["read_counts_by_edge"]["total:REV"].asUInt64(), 15ull);

    ASSERT_EQ(output["read_counts_by_node"]["total"].asUInt64(), 29ull);
    ASSERT_EQ(output["read_counts_by_node"]["total:FWD"].asUInt64(), 14ull);
    ASSERT_EQ(output["read_counts_by_node"]["total:REV"].asUInt64(), 15ull);

    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total"].asUInt64(), 15ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:FWD"].asUInt64(), 8ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:REV"].asUInt64(), 7ull);

    ASSERT_EQ(output["read_counts_by_sequence"]["ALT"]["total"].asUInt64(), 14ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["ALT"]["total:FWD"].asUInt64(), 6ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["ALT"]["total:REV"].asUInt64(), 8ull);
}

TEST(Paragraph, AlignsPGLongDel)
{
    auto logger = LOG();

    std::string bam_path
        = g_testenv->getBasePath() + "/../share/test-data/paragraph/long-del/chr4-21369091-21376907.bam";
    std::string graph_spec_path
        = g_testenv->getBasePath() + "/../share/test-data/paragraph/long-del/chr4-21369091-21376907.json";

    std::string reference_path = g_testenv->getHG19Path();
    if (reference_path.empty())
    {
        logger->warn("Warning: cannot do round-trip testing for paragraph without hg19 reference file -- please "
                     "specify a location using the HG19 environment variable.");
        return;
    }

    Parameters parameters(10000, 3, 0.01f, 0.8f, Parameters::output_options::ALL, true);

    parameters.load(bam_path, graph_spec_path, reference_path);

    common::ReadBuffer all_reads;
    common::extractReads(
        parameters.bam_path(), reference_path, parameters.target_regions(), (int)parameters.max_reads(), all_reads);
    const auto output = alignAndDisambiguate(parameters, all_reads);

    ASSERT_EQ(output["read_counts_by_edge"]["total"].asUInt64(), 201ull);
    ASSERT_EQ(output["read_counts_by_edge"]["total:READS"].asUInt64(), 210ull);
    ASSERT_EQ(output["read_counts_by_edge"]["total:FWD"].asUInt64(), 104ull);
    ASSERT_EQ(output["read_counts_by_edge"]["total:REV"].asUInt64(), 106ull);

    ASSERT_EQ(output["read_counts_by_node"]["total"].asUInt64(), 201ull);
    ASSERT_EQ(output["read_counts_by_node"]["total:READS"].asUInt64(), 210ull);
    ASSERT_EQ(output["read_counts_by_node"]["total:FWD"].asUInt64(), 104ull);
    ASSERT_EQ(output["read_counts_by_node"]["total:REV"].asUInt64(), 106ull);

    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total"].asUInt64(), 149ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:READS"].asUInt64(), 158ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:FWD"].asUInt64(), 78ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:REV"].asUInt64(), 80ull);

    ASSERT_FALSE(output["read_counts_by_sequence"].isMember("ALT"));
}

TEST(Paragraph, CountsClippedReads)
{
    auto logger = LOG();

    std::string bam_path = g_testenv->getBasePath() + "/../share/test-data/paragraph/quantification/NA12878-chr6.bam";
    std::string graph_spec_path
        = g_testenv->getBasePath() + "/../share/test-data/paragraph/quantification/chr6-53037879-53037949.vcf.json";

    std::string reference_path = g_testenv->getHG19Path();
    if (reference_path.empty())
    {
        logger->warn("Warning: cannot do round-trip testing for paragraph without hg19 reference file -- please "
                     "specify a location using the HG19 environment variable.");
        return;
    }

    Parameters parameters(10000, 3, 0.01f, 0.8f, Parameters::output_options::ALL, true);

    parameters.load(bam_path, graph_spec_path, reference_path);

    common::ReadBuffer all_reads;
    common::extractReads(
        parameters.bam_path(), reference_path, parameters.target_regions(), (int)parameters.max_reads(), all_reads);
    const auto output = alignAndDisambiguate(parameters, all_reads);

    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total"].asUInt64(), 12ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:FWD"].asUInt64(), 6ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["REF"]["total:REV"].asUInt64(), 6ull);

    ASSERT_EQ(output["read_counts_by_sequence"]["ALT"]["total"].asUInt64(), 1ull);
    ASSERT_EQ(output["read_counts_by_sequence"]["ALT"]["total:REV"].asUInt64(), 1ull);
}