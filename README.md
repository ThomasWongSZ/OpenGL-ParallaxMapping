# OpenGL - Parallax Mapping
使用QtCreator开发，皆在演示使用QOpenGLWidget的使用方法和法线贴图+视差贴图的使用方法。

使用方法：
1.运行程序。
2.移动鼠标，查看光源照射到木块时的效果。
框架： QMainWindow加入一个QOpenGLWidget。 使用一个法线贴图和视差贴图，并在vertex shader中计算光照。法线定义在TBN坐标系里。
