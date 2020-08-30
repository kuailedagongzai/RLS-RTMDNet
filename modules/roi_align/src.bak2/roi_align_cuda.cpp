//#include <THC/THC.h>
//#include <ATen/ATen.h>
#include "roi_align_cuda.hpp"
#include <math.h>
#include "cuda/roi_align_kernel.h"
//#include <torch/extension.h>

THC_CLASS at::Context& at::globalContext();
THCState *state = at::globalContext().getTHCState();
//ATen_CLASS at::Context& at::globalContext();
//THCState *state = at::globalContext().thc_state;
//extern THCState *state;

extern "C" int roi_align_forward_cuda(int aligned_height, int aligned_width, float spatial_scale,
                        at::Tensor& features_t, at::Tensor& rois_t, at::Tensor& output_t)
{
    //printf("%d\n", aligned_height);
    THCudaTensor * features(std::move(features_t.unsafeGetTensorImpl()));
    THCudaTensor * rois(std::move(rois_t.unsafeGetTensorImpl()));
    THCudaTensor * output(std::move(output_t.unsafeGetTensorImpl()));
    THCudaTensor * features_ = THCudaTensor_newContiguous(state, features);
    THCudaTensor * rois_ = THCudaTensor_newContiguous(state, rois);
    THCudaTensor * output_ = THCudaTensor_newContiguous(state, output);
    THCudaStorage * features__ = THCudaTensor_storage(state, features_);
    THCudaStorage * rois__ = THCudaTensor_storage(state, rois_);
    THCudaStorage * output__ = THCudaTensor_storage(state, output_);
    printf("%d\n", aligned_width);
    //features = THCudaTensor_newContiguous(state, features_t.impl_.get());
    // Grab the input tensor
    float * data_flat = THCudaStorage_data(state, features__);
    printf("%d\n", aligned_height);
    float * rois_flat = THCudaStorage_data(state, rois__);

    float * output_flat = THCudaStorage_data(state, output__);
    printf("%d\n", aligned_height);
    // Number of ROIs
    int num_rois = THCudaTensor_size(state, rois, 0);
    int size_rois = THCudaTensor_size(state, rois, 1);
    if (size_rois != 5)
    {
        return 0;
    }
    printf("%f\n", spatial_scale);
    // batch size
    //int batch_size = THCudaTensor_size(state, features, 0);
    //if (batch_size != 1)
    //{
    //    return 0;
    //}


    // data height
    int data_height = THCudaTensor_size(state, features, 2);
    // data width
    int data_width = THCudaTensor_size(state, features, 3);
    // Number of channels
    int num_channels = THCudaTensor_size(state, features, 1);

    cudaStream_t stream = THCState_getCurrentStream(state);

    ROIAlignForwardLaucher(
        data_flat, spatial_scale, num_rois, data_height,
        data_width, num_channels, aligned_height,
        aligned_width, rois_flat,
        output_flat, stream);

    return 1;
}

extern "C" int roi_align_backward_cuda(int aligned_height, int aligned_width, float spatial_scale,
                        THCudaTensor * top_grad, THCudaTensor * rois, THCudaTensor * bottom_grad)
{
    // Grab the input tensor
    float * top_grad_flat = THCudaTensor_data(state, top_grad);
    float * rois_flat = THCudaTensor_data(state, rois);

    float * bottom_grad_flat = THCudaTensor_data(state, bottom_grad);

    // Number of ROIs
    int num_rois = THCudaTensor_size(state, rois, 0);
    int size_rois = THCudaTensor_size(state, rois, 1);
    if (size_rois != 5)
    {
        return 0;
    }

    // batch size
    int batch_size = THCudaTensor_size(state, bottom_grad, 0);
    //if (batch_size != 1)
    //{
     //   return 0;
    //}
    // data height
    int data_height = THCudaTensor_size(state, bottom_grad, 2);
    // data width
    int data_width = THCudaTensor_size(state, bottom_grad, 3);
    // Number of channels
    int num_channels = THCudaTensor_size(state, bottom_grad, 1);

    cudaStream_t stream = THCState_getCurrentStream(state);
    ROIAlignBackwardLaucher(
        top_grad_flat, spatial_scale, batch_size, num_rois, data_height,
        data_width, num_channels, aligned_height,
        aligned_width, rois_flat,
        bottom_grad_flat, stream);

    return 1;
}


extern "C" int roi_align_ada_forward_cuda(int aligned_height, int aligned_width, float spatial_scale,
                        THCudaTensor * features, THCudaTensor * rois, THCudaTensor * output)
{
    // Grab the input tensor
    float * data_flat = THCudaTensor_data(state, features);
    float * rois_flat = THCudaTensor_data(state, rois);

    float * output_flat = THCudaTensor_data(state, output);

    // Number of ROIs
    int num_rois = THCudaTensor_size(state, rois, 0);
    int size_rois = THCudaTensor_size(state, rois, 1);
    if (size_rois != 5)
    {
        return 0;
    }

    // batch size
    //int batch_size = THCudaTensor_size(state, features, 0);
    //if (batch_size != 1)
    //{
    //    return 0;
    //}


    // data height
    int data_height = THCudaTensor_size(state, features, 2);
    // data width
    int data_width = THCudaTensor_size(state, features, 3);
    // Number of channels
    int num_channels = THCudaTensor_size(state, features, 1);

    cudaStream_t stream = THCState_getCurrentStream(state);

    ROIAlignAdaForwardLaucher(
        data_flat, spatial_scale, num_rois, data_height,
        data_width, num_channels, aligned_height,
        aligned_width, rois_flat,
        output_flat, stream);

    return 1;
}

extern "C" int roi_align_ada_backward_cuda(int aligned_height, int aligned_width, float spatial_scale,
                        THCudaTensor * top_grad, THCudaTensor * rois, THCudaTensor * bottom_grad)
{
    // Grab the input tensor
    float * top_grad_flat = THCudaTensor_data(state, top_grad);
    float * rois_flat = THCudaTensor_data(state, rois);

    float * bottom_grad_flat = THCudaTensor_data(state, bottom_grad);

    // Number of ROIs
    int num_rois = THCudaTensor_size(state, rois, 0);
    int size_rois = THCudaTensor_size(state, rois, 1);
    if (size_rois != 5)
    {
        return 0;
    }

    // batch size
    int batch_size = THCudaTensor_size(state, bottom_grad, 0);
    //if (batch_size != 1)
    //{
     //   return 0;
    //}
    // data height
    int data_height = THCudaTensor_size(state, bottom_grad, 2);
    // data width
    int data_width = THCudaTensor_size(state, bottom_grad, 3);
    // Number of channels
    int num_channels = THCudaTensor_size(state, bottom_grad, 1);

    cudaStream_t stream = THCState_getCurrentStream(state);
    ROIAlignAdaBackwardLaucher(
        top_grad_flat, spatial_scale, batch_size, num_rois, data_height,
        data_width, num_channels, aligned_height,
        aligned_width, rois_flat,
        bottom_grad_flat, stream);

    return 1;
}


extern "C" int roi_align_dense_ada_forward_cuda(int aligned_height, int aligned_width, float spatial_scale,
                        THCudaTensor * features, THCudaTensor * rois, THCudaTensor * output)
{
    // Grab the input tensor
    float * data_flat = THCudaTensor_data(state, features);
    float * rois_flat = THCudaTensor_data(state, rois);

    float * output_flat = THCudaTensor_data(state, output);

    // Number of ROIs
    int num_rois = THCudaTensor_size(state, rois, 0);
    int size_rois = THCudaTensor_size(state, rois, 1);
    if (size_rois != 5)
    {
        return 0;
    }

    // batch size
    //int batch_size = THCudaTensor_size(state, features, 0);
    //if (batch_size != 1)
    //{
    //    return 0;
    //}


    // data height
    int data_height = THCudaTensor_size(state, features, 2);
    // data width
    int data_width = THCudaTensor_size(state, features, 3);
    // Number of channels
    int num_channels = THCudaTensor_size(state, features, 1);

    cudaStream_t stream = THCState_getCurrentStream(state);

    ROIAlignDenseAdaForwardLaucher(
        data_flat, spatial_scale, num_rois, data_height,
        data_width, num_channels, aligned_height,
        aligned_width, rois_flat,
        output_flat, stream);

    return 1;
}

extern "C" int roi_align_dense_ada_backward_cuda(int aligned_height, int aligned_width, float spatial_scale,
                        THCudaTensor * top_grad, THCudaTensor * rois, THCudaTensor * bottom_grad)
{
    // Grab the input tensor
    float * top_grad_flat = THCudaTensor_data(state, top_grad);
    float * rois_flat = THCudaTensor_data(state, rois);

    float * bottom_grad_flat = THCudaTensor_data(state, bottom_grad);

    // Number of ROIs
    int num_rois = THCudaTensor_size(state, rois, 0);
    int size_rois = THCudaTensor_size(state, rois, 1);
    if (size_rois != 5)
    {
        return 0;
    }

    // batch size
    int batch_size = THCudaTensor_size(state, bottom_grad, 0);
    //if (batch_size != 1)
    //{
     //   return 0;
    //}
    // data height
    int data_height = THCudaTensor_size(state, bottom_grad, 2);
    // data width
    int data_width = THCudaTensor_size(state, bottom_grad, 3);
    // Number of channels
    int num_channels = THCudaTensor_size(state, bottom_grad, 1);

    cudaStream_t stream = THCState_getCurrentStream(state);
    ROIAlignDenseAdaBackwardLaucher(
        top_grad_flat, spatial_scale, batch_size, num_rois, data_height,
        data_width, num_channels, aligned_height,
        aligned_width, rois_flat,
        bottom_grad_flat, stream);

    return 1;
}

/*PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
  m.def("roi_align_forward_cuda", &roi_align_forward_cuda, "ROIAlign_forward");
  m.def("roi_align_backward_cuda", &roi_align_backward_cuda, "ROIAlign_backward");
  m.def("roi_align_ada_forward_cuda", &roi_align_ada_forward_cuda, "ROIAlign_Ada_forward");
  m.def("roi_align_ada_backward_cuda", &roi_align_ada_backward_cuda, "ROIAlign_Ada_backward");
  m.def("roi_align_dense_ada_forward_cuda", &roi_align_dense_ada_forward_cuda, "ROIAlign_Dense_Ada_forward");
  m.def("roi_align_dense_ada_backward_cuda", &roi_align_dense_ada_backward_cuda, "ROIAlign_Dense_Ada_backward");
}*/
