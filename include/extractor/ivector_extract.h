//
// Created by coast on 18-11-11.
//

#ifndef KALDI_IVECTOR_EXTRACT_H
#define KALDI_IVECTOR_EXTRACT_H

#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "ivector/ivector-extractor.h"
#include "util/kaldi-thread.h"


namespace kaldi {
    void ReadExtractor(const std::string& filepath, IvectorExtractor* extractor);
    void ExtractIvector(const IvectorExtractor &extractor,
                        std::string &utt,
                        const Matrix<BaseFloat> &feats,
                        const Posterior &posterior,
                        BaseFloatVectorWriter *writer,
                        double *tot_auxf_change,
                        Vector<double> &ivector);
}
#endif //KALDI_IVECTOR_EXTRACT_H
