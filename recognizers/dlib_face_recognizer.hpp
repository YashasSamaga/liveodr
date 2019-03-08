#ifndef LIVEODR_RECOGNIZERS_DLIB_FACE_RECOGNIZER_HPP
#define LIVEODR_RECOGNIZERS_DLIB_FACE_RECOGNIZER_HPP

#include "recognizer.hpp"

#include "../utils.hpp"

#include <dlib/dnn.h>
#include <dlib/opencv.h>

#include <string_view>
#include <string>
#include <vector>

namespace liveodr {
    class DlibFaceRecognizer : virtual public IRecognizer {
    public:
        DlibFaceRecognizer() {
            load("data/dlib_face_recognition_resnet_model_v1.dat", "data/shape_predictor_5_face_landmarks.dat");
        }

        DlibFaceRecognizer(std::string_view net_model, std::string_view sp_model) {
            load(net_model, sp_model);
        }

        void load(std::string_view net_model, std::string_view sp_model) {
            dlib::deserialize(std::string(net_model)) >> net;
            dlib::deserialize(std::string(sp_model)) >> sp;
        }

        const result_type recognize(const cv::Mat sample, cv::Rect rect) override {
            std::vector<cv::Rect> objects;
            objects.push_back(rect);
            return recognize(sample, objects)[0];
        } 

        const std::vector<result_type> recognize(const cv::Mat sample, std::vector<cv::Rect> objects) override {
            dlib::cv_image<dlib::bgr_pixel> converted_sample(sample);
            
            std::vector<dlib::matrix<dlib::rgb_pixel>> faces;
            for(auto& rect : objects) {
                auto shape = sp(converted_sample, liveodr::utils::opencv_to_dlib_rect(rect));
                dlib::matrix<dlib::rgb_pixel>  face_chip;
                dlib::extract_image_chip(converted_sample, dlib::get_face_chip_details(shape,150,0.25), face_chip);
                faces.push_back(face_chip);
            }       

            std::vector<dlib::matrix<float, 0, 1>> face_descriptors = net(faces);

            std::vector<result_type> result;
            for (auto& face : face_descriptors) {
                result.push_back({"recognized", 1.0});
            }

            return result;
        }

    private:
        template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
        using residual = dlib::add_prev1<block<N,BN,1,dlib::tag1<SUBNET>>>;

        template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
        using residual_down = dlib::add_prev2<dlib::avg_pool<2,2,2,2,dlib::skip1<dlib::tag2<block<N,BN,2,dlib::tag1<SUBNET>>>>>>;

        template <int N, template <typename> class BN, int stride, typename SUBNET> 
        using block  = BN<dlib::con<N,3,3,1,1,dlib::relu<BN<dlib::con<N,3,3,stride,stride,SUBNET>>>>>;

        template <int N, typename SUBNET> using ares      = dlib::relu<residual<block,N,dlib::affine,SUBNET>>;
        template <int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block,N,dlib::affine,SUBNET>>;

        template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
        template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
        template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
        template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
        template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

        using anet_type = dlib::loss_metric<dlib::fc_no_bias<128,dlib::avg_pool_everything<
                                    alevel0<
                                    alevel1<
                                    alevel2<
                                    alevel3<
                                    alevel4<
                                    dlib::max_pool<3,3,2,2,dlib::relu<dlib::affine<dlib::con<32,7,7,2,2,
                                    dlib::input_rgb_image_sized<150>
                                    >>>>>>>>>>>>;
        anet_type net;
        dlib::shape_predictor sp;
    };
}

#endif /* ifndef LIVEODR_RECOGNIZERS_DLIB_FACE_RECOGNIZER_HPP */