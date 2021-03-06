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
 * Genotyper for the graph-represented whole variants with multiple breakpoints.
 * Most likely genotype is voted from all breakpoint genotypes.
 *
 * \author Sai Chen & Egor Dolzhenko & Peter Krusche
 * \email schen6@illumina.com & pkrusche@illumina.com & edolzhenko@illumina.com
 *
 */

#pragma once

#include <cstdint>

#include "GraphGenotyper.hh"
#include "genotyping/Genotype.hh"
#include "genotyping/GenotypingParameters.hh"

namespace genotyping
{
class GraphBreakpointGenotyper : public GraphGenotyper
{
public:
    GraphBreakpointGenotyper(unsigned int male_ploidy = 2, unsigned int female_ploidy = 2)
        : male_ploidy_(male_ploidy)
        , female_ploidy_(female_ploidy){};

    /**
     * set genotyping parameters from JSON
     */
    void setParameters(const std::string& genotyping_parameter_path) override;

protected:
    /**
     * Implemented by derived classes which implement genotyping
     */
    void runGenotyping() override;

private:
    /**
     * get ploidy according to its sex
     */
    unsigned int getSamplePloidy(size_t sample_index);

    /**
     * genotyping parameters
     */
    std::unique_ptr<GenotypingParameters> p_genotype_parameter;
    std::unique_ptr<GenotypingParameters> p_male_genotype_parameter;

    unsigned int male_ploidy_;
    unsigned int female_ploidy_;
};
}
