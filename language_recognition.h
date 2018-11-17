//
// Created by coast on 18-11-17.
//

#ifndef KALDI_LANGUAGE_RECOGNITION_H
#define KALDI_LANGUAGE_RECOGNITION_H
#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "feat/feature-mfcc.h"
#include "feat/wave-reader.h"
#include "ivector/voice-activity-detection.h"
#include "gmm/diag-gmm.h"
#include "gmm/full-gmm.h"
#include "hmm/posterior.h"
#include "ivector/ivector-extractor.h"
#include "ivector/logistic-regression.h"
using namespace kaldi;

void compute_compressed_mfcc(std::string wav_rspecifier, CompressedMatrix* compressedmatrix);
void compute_vad(CompressedMatrix feat, Vector<BaseFloat>* vad_result);
void apply_cmvn_sliding(CompressedMatrix feat, Matrix<BaseFloat>* cmvn_feat);
void add_deltas_sdc(Matrix<BaseFloat>input_feat, Matrix<BaseFloat>* new_feats);
void select_voiced_frames(int dim, Matrix<BaseFloat>* feat,Vector<BaseFloat>* voiced,Matrix<BaseFloat>* voiced_feat );
void gmm_gselect(std::string& model_filename, Matrix<BaseFloat>* mat,std::vector<std::vector<int32>>* gselect);
void fgmm_global_gselect_to_post(std::string& model_filename,Matrix<BaseFloat>* mat,std::vector<std::vector<int32>>* gselect, Posterior* post);
void read_extractor(const std::string& filepath, IvectorExtractor* extractor);
void extract_ivector(const IvectorExtractor &extractor,
                     const Matrix<BaseFloat> &feats,
                     const Posterior &posterior,
                     Vector<double>* ivector);
void ivector_norm_length(Vector<BaseFloat>* ivector);
void logistic_regression_eval(std::string& lr_model_path, Vector<BaseFloat>* ivector, Vector<BaseFloat>* log_posteriors);

#endif //KALDI_LANGUAGE_RECOGNITION_H
