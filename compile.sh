###### final ######
# compile the dynamic library:
g++ -std=c++11   -DHAVE_ATLAS -pthread -c -o language_recognition.o language_recognition.cc -I include/ -fPIC
g++ -shared -fPIC -o so/liblanguage_recognition.so language_recognition.o  ../../ivector/{ivector-extractor.o,voice-activity-detection.o,plda.o,logistic-regression.o} ../../hmm/{hmm-topology.o,transition-model.o,hmm-utils.o,tree-accu.o,posterior.o,hmm-test-utils.o}  ../../gmm/{diag-gmm.o,diag-gmm-normal.o,mle-diag-gmm.o,am-diag-gmm.o,mle-am-diag-gmm.o,full-gmm.o,full-gmm-normal.o,mle-full-gmm.o,model-common.o,decodable-am-diag-gmm.o,model-test-common.o,ebw-diag-gmm.o,indirect-diff-diag-gmm.o} ../../tree/{event-map.o,context-dep.o,clusterable-classes.o,cluster-utils.o,build-tree-utils.o,build-tree.o,build-tree-questions.o,tree-renderer.o} ../../util/{text-utils.o,kaldi-io.o,kaldi-holder.o,kaldi-table.o,parse-options.o,simple-options.o,simple-io-funcs.o,kaldi-semaphore.o,kaldi-thread.o} ../../matrix/{kaldi-matrix.o,kaldi-vector.o,packed-matrix.o,sp-matrix.o,tp-matrix.o,matrix-functions.o,qr.o,srfft.o,compressed-matrix.o,sparse-matrix.o,optimization.o}   ../../base/{kaldi-math.o,kaldi-error.o,io-funcs.o,kaldi-utils.o} ../../feat/*.o ../../transform/*.o

# use the dynamic library:
all the headers depended are put in include
add so:so/dep:so/dep/kaldi_so to LD_LIBRARY_PATH
g++ -std=c++11   -DHAVE_ATLAS -pthread -c -o test.o test.cpp -I include/
g++ -o test test.o  -pthread -L so/dep/kaldi_so/ -lkaldi-util  -lkaldi-matrix -lkaldi-hmm -L so -l language_recognition
######











# all the try
g++ -std=c++11 -I.. -I/home/coast/kaldi/tools/openfst/include -Wno-sign-compare -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_ATLAS -I/home/coast/kaldi/tools/ATLAS/include -msse -msse2 -pthread -g  -fPIC -DKALDI_NO_EXPF   -c -o ivector-extract.o ivector-extract.cc
g++  -Wl,-rpath=/home/coast/kaldi/tools/openfst/lib -rdynamic -Wl,-rpath=/home/coast/kaldi/src/lib  ivector-extract.o   ../ivector/libkaldi-ivector.so  ../hmm/libkaldi-hmm.so  ../gmm/libkaldi-gmm.so  ../tree/libkaldi-tree.so  ../util/libkaldi-util.so  ../matrix/libkaldi-matrix.so  ../base/libkaldi-base.so /home/coast/kaldi/tools/openfst/lib/libfst.so /usr/lib/libatlas.so.3 /usr/lib/libf77blas.so.3 /usr/lib/libcblas.so.3 /usr/lib/liblapack_atlas.so.3 -lm -lpthread -ldl -o ivector-extract




g++  -Wl,-rpath=/home/coast/kaldi/tools/openfst/lib   ivector_extract.o ivector_extract_test.o so/dep/kaldi_so/libkaldi-ivector.so  so/dep/kaldi_so/libkaldi-hmm.so  so/dep/kaldi_so/libkaldi-gmm.so  so/dep/kaldi_so/libkaldi-tree.so  so/dep/kaldi_so/libkaldi-util.so  so/dep/kaldi_so/libkaldi-matrix.so  so/dep/kaldi_so/libkaldi-base.so -rdynamic -Wl,-rpath=/home/coast/kaldi/src/ivectorbin/so/dep  -lm -lpthread -ldl -o ivector_extract_test
g++ -std=c++11 -I.. -I/home/coast/kaldi/tools/openfst/include -Wno-sign-compare -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_ATLAS -I/home/coast/kaldi/tools/ATLAS/include -msse -msse2 -pthread -g  -fPIC -DKALDI_NO_EXPF   -c -o ivector_extract_test.o ivector_extract_test.cpp
g++ -std=c++11 -I.. -I/root/kaldi/tools/openfst/include -Wno-sign-compare -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_ATLAS -I/root/kaldi/tools/ATLAS/include -msse -msse2 -pthread -g  -fPIC -DKALDI_NO_EXPF   -c -o ivector_extract.o ivector_extract.cpp
g++ ivector_extract.o ../lib/libkaldi-ivector.so  ../lib/libkaldi-hmm.so  ../lib/libkaldi-gmm.so  ../lib/libkaldi-tree.so  ../lib/libkaldi-util.so  ../lib/libkaldi-matrix.so  ../lib/libkaldi-base.so -Wl,-rpath=/root/kaldi/src/ivectorbin/so/dep -o so/libextractor.so -shared -fPIC
g++ -o ivector_extract_test.o ivector_extract_test.cpp -I .. -std=c++11 -I /root/kaldi/tools/openfst/include/ -I /root/kaldi/tools/ATLAS_headers/include/ -c
g++ ivector_extract.o ../ivector/kaldi-ivector.a  ../hmm/kaldi-hmm.a ../gmm/kaldi-gmm.a  ../tree/kaldi-tree.a  ../util/kaldi-util.a ../matrix/kaldi-matrix.a ../base/kaldi-base.a -Wl,-rpath=/root/kaldi/src/ivectorbin/so/dep -o so/libextractor.so -shared -fPIC


g++ -o ivector_extract_test.o ivector_extract_test.cpp -std=c++11 -c -I include/
g++ -o ivector_extract_test ivector_extract.o -L so -l extractor -L so/dep/ -l cblas -L so/dep -l lapack_atlas





# desirable

g++ -std=c++11  -Wno-sign-compare -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_ATLAS -msse -msse2 -pthread -g  -fPIC -DKALDI_NO_EXPF   -c -o ivector_extract.o ivector_extract.cpp  -I include/  -I include/clapack/ -I include/cblas
g++ -o so/libextractor.so ivector_extract.o -L so/dep/kaldi_so/ -lkaldi-ivector -L so/dep/kaldi_so/ -l kaldi-hmm -L so/dep/kaldi_so/ -lkaldi-gmm -L so/dep/kaldi_so/ -lkaldi-tree -L so/dep/kaldi_so/ -lkaldi-util -L so/dep/kaldi_so/ -lkaldi-matrix -L so/dep/kaldi_so/ -lkaldi-base -shared -fPIC

g++ -std=c++11  -Wno-sign-compare -Wall -Wno-sign-compare -Wno-unused-local-typedefs -Wno-deprecated-declarations -Winit-self -DKALDI_DOUBLEPRECISION=0 -DHAVE_EXECINFO_H=1 -DHAVE_CXXABI_H -DHAVE_ATLAS -msse -msse2 -pthread -g  -fPIC -DKALDI_NO_EXPF   -c -o ivector_extract_test.o ivector_extract_test.cpp  -I include/  -I include/clapack/ -I include/cblas/
#g++    ivector_extract_test.o ivector_extract.o   ../ivector/libkaldi-ivector.so  ../hmm/libkaldi-hmm.so  ../gmm/libkaldi-gmm.so  ../tree/libkaldi-tree.so  ../util/libkaldi-util.so  ../matrix/libkaldi-matrix.so  ../base/libkaldi-base.so -Wl,-rpath=so/dep -lpthread  -o ivector_extract_tes

g++    ivector_extract_test.o -o ivector_extract_test -pthread -L so/ -l extractor -L so/dep/kaldi_so/ -lkaldi-ivector -l kaldi-hmm -lkaldi-gmm -lkaldi-tree -lkaldi-util -lkaldi-matrix -lkaldi-base -Wl,-rpath=so/dep
