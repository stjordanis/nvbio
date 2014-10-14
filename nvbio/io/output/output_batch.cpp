/*
 * nvbio
 * Copyright (c) 2011-2014, NVIDIA CORPORATION. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the NVIDIA CORPORATION nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <nvbio/io/output/output_batch.h>
#include <nvbio/basic/timer.h>
#include <nvbio/basic/vector.h>

#include <stdio.h>
#include <stdarg.h>

namespace nvbio {
namespace io {

// copy scoring data to host, converting to io::AlignmentResult
void DeviceOutputBatchSE::readback_scores(thrust::host_vector<io::Alignment>& host_alignments) const
{
    // copy alignment data into a staging buffer
    host_alignments = alignments;
}

// copy CIGARs into host memory
void DeviceOutputBatchSE::readback_cigars(io::HostCigarArray& host_cigar) const
{
    host_cigar.array = cigar.array;
    nvbio::cuda::thrust_copy_vector(host_cigar.coords, cigar.coords);
}

// copy MD strings back to the host
void DeviceOutputBatchSE::readback_mds(nvbio::HostVectorArray<uint8>& host_mds) const
{
    host_mds = mds;
}

// copy mapq back to the host
void DeviceOutputBatchSE::readback_mapq(thrust::host_vector<uint8>& host_mapq) const
{
    host_mapq = mapq;
}

// extract alignment data for a given mate
// note that the mates can be different for the cigar, since mate 1 is always the anchor mate for cigars
AlignmentData HostOutputBatchPE::get_mate(uint32 read_id, AlignmentMate mate)
{
    if (alignments[0][read_id].mate() == mate)
    {
        return AlignmentData(&alignments[0][read_id],
                             mapq[read_id],
                             read_id,
                             read_data[ mate ],
                             &cigar[0],
                             &mds[0]);
    }
    else
    {
        return AlignmentData(&alignments[1][read_id],
                             mapq[read_id],
                             read_id,
                             read_data[ mate ],
                             &cigar[1],
                             &mds[1]);
    }
}

// extract alignment data for the anchor mate
AlignmentData HostOutputBatchPE::get_anchor_mate(uint32 read_id)
{
    const uint32 mate = alignments[0][read_id].mate();
    return AlignmentData(&alignments[0][read_id],
                         mapq[read_id],
                         read_id,
                         read_data[ mate ],
                         &cigar[0],
                         &mds[0]);
}

// extract alignment data for the opposite mate
AlignmentData HostOutputBatchPE::get_opposite_mate(uint32 read_id)
{
    const uint32 mate = alignments[1][read_id].mate();
    return AlignmentData(&alignments[1][read_id],
                         mapq[read_id],
                         read_id,
                         read_data[ mate ],
                         &cigar[1],
                         &mds[1]);
}

} // namespace io
} // namespace nvbio
