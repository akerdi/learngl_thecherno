# 课程7 出现的问题

## syntax error #version

Macos 需要增加:

```
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  cout << "I'm apple machine" << endl;
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
```

以上升shader版本，否则会使用2.1版本，在编译shader时，会报 `syntax error #version`

## 图形展示不出

需要增加VAO，否则无法展示正确展示shader(原理不理解)