#include "language_recognition.h"

using namespace kaldi;

void compute_compressed_mfcc(std::string wav_rspecifier, CompressedMatrix* compressedmatrix){
  try {
    MfccOptions mfcc_opts;
    BaseFloat vtln_warp = 1.0;

    int32 this_chan = 0;
    int32 channel = -1;
    BaseFloat min_duration = 0.0;

    Mfcc mfcc(mfcc_opts);

    std::ifstream is;
    is.open(wav_rspecifier.c_str(), std::ios_base::in|std::ios_base::binary);
    WaveData wave_data;
    wave_data.Read(is);
    if (wave_data.Duration() < min_duration) {
      KALDI_WARN << "File:  is too short ("
                 << wave_data.Duration() << " sec): producing no output.";
    }

    int32 num_chan = wave_data.Data().NumRows();
    SubVector<BaseFloat> waveform(wave_data.Data(), this_chan);
    Matrix<BaseFloat> features;
    try {
      mfcc.ComputeFeatures(waveform, wave_data.SampFreq(), vtln_warp, &features);
    } catch (...) {
      KALDI_WARN << "Failed to compute features for utterance";
    }

    BaseFloatMatrixWriter kaldi_writer("ark:new/mfcc.ark");
    kaldi_writer.Write("01", features);
    kaldi_writer.Close();
    CompressionMethod compression_method = static_cast<CompressionMethod>(1);
    *compressedmatrix = CompressedMatrix(features,compression_method);
  } catch(const std::exception &e) {
    std::cerr << e.what();
  }
}

void compute_vad(CompressedMatrix feat, Vector<BaseFloat>* vad_result){
  try {
    VadEnergyOptions opts;
    int32 num_done = 0, num_err = 0;
    int32 num_unvoiced = 0;
    double tot_length = 0.0, tot_decision = 0.0;

      if (feat.NumRows() == 0) {
        KALDI_WARN << "Empty feature matrix for utterance "; // << utt;
        num_err++;
      }


      ComputeVadEnergy(opts, (Matrix<BaseFloat>)feat, vad_result);

      double sum = vad_result->Sum();
      if (sum == 0.0) {
        KALDI_WARN << "No frames were judged voiced for utterance "; // << utt;
        num_unvoiced++;
      } else {
        num_done++;
      }
      tot_decision += vad_result->Sum();
      tot_length += vad_result->Dim();
  } catch(const std::exception &e) {
    std::cerr << e.what();
  }
}

void apply_cmvn_sliding(CompressedMatrix feat, Matrix<BaseFloat>* cmvn_feat) {
  SlidingWindowCmnOptions opts;
  SlidingWindowCmn(opts, (Matrix<BaseFloat>)feat, cmvn_feat);
}

void add_deltas_sdc(Matrix<BaseFloat>input_feat, Matrix<BaseFloat>* new_feats) {
  ShiftedDeltaFeaturesOptions opts;     // no modification
  ComputeShiftedDeltas(opts, input_feat, new_feats);
}

void select_voiced_frames(int dim, Matrix<BaseFloat>* feat,Vector<BaseFloat>* voiced,Matrix<BaseFloat>* voiced_feat ) {
  int32 index = 0;
  for (int32 i = 0; i < feat->NumRows(); i++) {
    if ((*voiced)(i) != 0.0) {
      KALDI_ASSERT((*voiced)(i) == 1.0); // should be zero or one.
      voiced_feat->Row(index).CopyFromVec(feat->Row(i));
      index++;
    }
  }
  KALDI_ASSERT(index == dim);

}

void gmm_gselect(std::string& model_filename, Matrix<BaseFloat>* mat,std::vector<std::vector<int32>>* gselect) {
  int32 num_gselect = 20;
  DiagGmm gmm;
  //ReadKaldiObject(model_filename, &gmm);
  std::ifstream is;
  is.open(model_filename.c_str(), std::ios_base::in|std::ios_base::binary);
  is.get();
  is.get();
  gmm.Read(is, true);
  int32 num_gauss = gmm.NumGauss();
  if (num_gselect > num_gauss) {
    KALDI_WARN << "You asked for " << num_gselect << " Gaussians but GMM "
               << "only has " << num_gauss << ", returning this many. "
               << "Note: this means the Gaussian selection is pointless.";
    num_gselect = num_gauss;
  }
  double tot_like_this_file = 0;
  tot_like_this_file =
    gmm.GaussianSelection(*mat, num_gselect, gselect);
}
void fgmm_global_gselect_to_post(std::string& model_filename,Matrix<BaseFloat>* mat,std::vector<std::vector<int32>>* gselect, Posterior* post) {
  BaseFloat min_post = 0.025;
  FullGmm fgmm;
  std::ifstream is;
  is.open(model_filename.c_str(), std::ios_base::in|std::ios_base::binary);
  is.get();
  is.get();
  fgmm.Read(is, true);

  int32 num_frames = mat->NumRows();
  // typedef std::vector<std::vector<std::pair<int32, BaseFloat> > > Posterior;

  double this_tot_loglike = 0;
  for (int32 t = 0; t < num_frames; t++) {
    SubVector<BaseFloat> frame(*mat, t);
    const std::vector<int32> &this_gselect = (*gselect)[t];
    KALDI_ASSERT(!(*gselect)[t].empty());
    Vector<BaseFloat> loglikes;
    fgmm.LogLikelihoodsPreselect(frame, this_gselect, &loglikes);
    this_tot_loglike += loglikes.ApplySoftMax();
    // now "loglikes" contains posteriors.

    if (min_post != 0.0) {
      int32 max_index = 0; // in case all pruned away...
      loglikes.Max(&max_index);
      for (int32 i = 0; i < loglikes.Dim(); i++)
        if (loglikes(i) < min_post)
          loglikes(i) = 0.0;
        BaseFloat sum = loglikes.Sum();
        if (sum == 0.0) {
          loglikes(max_index) = 1.0;
        } else {
          loglikes.Scale(1.0 / sum);
        }
    }
    for (int32 i = 0; i < loglikes.Dim(); i++) {
      if (loglikes(i) != 0.0) {
        (*post)[t].push_back(std::make_pair(this_gselect[i], loglikes(i)));
      }
    }
    KALDI_ASSERT(!(*post)[t].empty());
  }
}

void read_extractor(const std::string& filepath, IvectorExtractor* extractor){
  std::ifstream is;
  is.open(filepath.c_str(), std::ios_base::in|std::ios_base::binary);
  is.get();
  is.get();
  extractor->Read(is, true);
}

void extract_ivector(const IvectorExtractor &extractor,
                    const Matrix<BaseFloat> &feats,
                    const Posterior &posterior,
                    Vector<double>* ivector){

  IvectorExtractorUtteranceStats utt_stats(extractor.NumGauss(),
                                           extractor.FeatDim(),
                                           false);
  utt_stats.AccStats(feats, posterior);

  ivector->Resize(extractor.IvectorDim());
  (*ivector)(0) = extractor.PriorOffset();

  extractor.GetIvectorDistribution(utt_stats, ivector, NULL);

  (*ivector)(0) -= extractor.PriorOffset();

}

void ivector_norm_length(Vector<BaseFloat>* ivector) {
  BaseFloat norm = ivector->Norm(2.0);
  BaseFloat ratio = norm / sqrt(ivector->Dim());
  if (ratio == 0.0) {
    KALDI_WARN << "Zero iVector";
  } else {
    ivector->Scale(1.0 / ratio);
  }
}

void logistic_regression_eval(std::string& lr_model_path, Vector<BaseFloat>* ivector, Vector<BaseFloat>* log_posteriors) {
  LogisticRegression classifier;
  std::ifstream is;
  is.open(lr_model_path.c_str(), std::ios_base::in|std::ios_base::binary);
  is.get();
  is.get();
  classifier.Read(is, true);

  classifier.GetLogPosteriors(*ivector, log_posteriors);
}
