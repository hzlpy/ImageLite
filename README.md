# ImageLite 轻型的影像处理项目

标签： 影像处理 

---

**目的**：建立一个轻型的影像处理项目。（暂时只含有影像特征提取，特征匹配，对齐影像）。
**技术**：C++, OpenCV, Qt

**Tips:**
OpenCV在项目中的配置：

 1. 在VC++目录下，配置包含目录"D:\opencv\build\include"；
 2. 在VC++目录下，配置库目录"D:\opencv\build\x86\vc10\lib"
 3. 在 链接器 ->输入 下配置 附加依赖项，如下：

    opencv_contrib248d.lib
    opencv_stitching248d.lib
    opencv_videostab248d.lib
    opencv_nonfree248d.lib
    opencv_superres248d.lib
    opencv_gpu248d.lib
    opencv_ocl248d.lib
    opencv_legacy248d.lib
    opencv_ts248d.lib
    opencv_calib3d248d.lib
    opencv_features2d248d.lib
    opencv_objdetect248d.lib
    opencv_highgui248d.lib
    opencv_photo248d.lib
    opencv_video248d.lib
    opencv_imgproc248d.lib
    opencv_flann248d.lib
    opencv_ml248d.lib
    opencv_core248d.lib




