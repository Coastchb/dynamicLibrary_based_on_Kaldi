//
// Created by coast on 18-11-17.
//
#include "language_recognition.h"
using namespace kaldi;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " wav_path" << std::endl;
        std::exit(1);
    }

    std::string wav_rspecifier = argv[1];
    std::string utt_id = "01";
    CompressedMatrixWriter kaldi_writer;  // typedef to TableWriter<something>.
    if (!kaldi_writer.Open("ark,scp:new/c1.ark,new/c1.scp"))
        KALDI_ERR << "Could not initialize output with wspecifier ";

    CompressedMatrix compressed_mfcc;
    compute_compressed_mfcc(wav_rspecifier,&compressed_mfcc);
    kaldi_writer.Write(utt_id, compressed_mfcc);
    kaldi_writer.Close();


    Vector<BaseFloat> vad_result(compressed_mfcc.NumRows());
    compute_vad(compressed_mfcc, &vad_result);
    BaseFloatVectorWriter vad_writer("ark,scp:new/v1.ark,new/v1.scp");
    vad_writer.Write(utt_id, vad_result);
    vad_writer.Close();

    Matrix<BaseFloat> cmvn_feat(compressed_mfcc.NumRows(),
                                compressed_mfcc.NumCols(), kUndefined);
    apply_cmvn_sliding(compressed_mfcc, &cmvn_feat);
    BaseFloatMatrixWriter cmvn_writer("ark:new/ap1.ark");
    cmvn_writer.Write(utt_id, cmvn_feat);
    cmvn_writer.Close();

    Matrix<BaseFloat> sdc_feat;
    add_deltas_sdc(cmvn_feat, &sdc_feat);
    BaseFloatMatrixWriter sdc_writer("ark:new/ad1.ark");
    sdc_writer.Write(utt_id, sdc_feat);
    sdc_writer.Close();

    int32 dim = 0;
    for (int32 i = 0; i < vad_result.Dim(); i++)
        if (vad_result(i) != 0.0)
            dim++;
    Matrix<BaseFloat> voiced_feat(dim, sdc_feat.NumCols());
    select_voiced_frames(dim, &sdc_feat, &vad_result, &voiced_feat);
    BaseFloatMatrixWriter voiced_writer("ark:new/sv1.ark");
    voiced_writer.Write(utt_id, voiced_feat);
    voiced_writer.Close();


    int32 num_voiced_frames = voiced_feat.NumRows();

    std::vector<std::vector<int32>> gselect(num_voiced_frames);
    std::string dubm_file = "final.dubm";
    gmm_gselect(dubm_file, &voiced_feat, &gselect);
    Int32VectorVectorWriter gselect_writer("ark:new/gselect1.ark");
    gselect_writer.Write(utt_id, gselect);
    gselect_writer.Close();

    Posterior post(num_voiced_frames);
    std::string ubm_file ="final.ubm";
    fgmm_global_gselect_to_post(ubm_file, &voiced_feat, &gselect, &post);
    PosteriorWriter post_writer("ark:new/fgselect1.ark");
    post_writer.Write(utt_id, post);
    post_writer.Close();

    PosteriorWriter posterior_writer("ark:new/sp1.ark");
    ScalePosterior(1.0, &post);
    posterior_writer.Write(utt_id, post);
    posterior_writer.Close();

    std::string extractor_filepath = "final.ie";
    IvectorExtractor extractor;
    read_extractor(extractor_filepath, &extractor);

    Vector<double> ivector;
    extract_ivector(extractor, voiced_feat, post, &ivector);
    BaseFloatVectorWriter ivector_writer("ark:new/ivector1.ark");
    ivector_writer.Write(utt_id, Vector<BaseFloat>(ivector));
    ivector_writer.Close();

    Vector<BaseFloat> ivector_float = Vector<BaseFloat>(ivector);
    ivector_norm_length(&ivector_float);
    BaseFloatVectorWriter nivector_writer("ark:new/nivector1.ark");
    nivector_writer.Write(utt_id, ivector_float);
    nivector_writer.Close();

    std::string lr_model_path = "logistic_regression_rebalanced";
    Vector<BaseFloat> log_posteriors;
    logistic_regression_eval(lr_model_path, &ivector_float, &log_posteriors);
    BaseFloatVectorWriter lr_posterior_writer("ark,t:new/posterior");
    lr_posterior_writer.Write(utt_id, log_posteriors);

}

