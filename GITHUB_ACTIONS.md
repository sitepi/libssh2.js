# GitHub Actions 自动构建说明

本项目已配置 GitHub Actions，可以自动编译 libssh2.js 并发布到 `dist/` 目录。

## 🚀 工作流功能

### 触发条件

工作流会在以下情况下自动运行：

1. **Push 到主分支**：推送代码到 `main` 或 `master` 分支
2. **Pull Request**：创建或更新针对主分支的 PR
3. **手动触发**：在 GitHub Actions 页面手动运行
4. **Tag 发布**：推送 tag 时会创建 GitHub Release

### 构建流程

1. ✅ **环境设置**
   - 安装 Node.js 18
   - 安装 Emscripten SDK 3.1.50
   
2. ✅ **依赖下载**（带缓存）
   - zlib 1.2.11
   - mbedtls 2.27.0
   - libssh2 1.10.0
   
3. ✅ **编译依赖库**
   - 编译 zlib（静态库）
   - 编译 mbedtls（静态库）
   - 编译 libssh2（静态库）
   
4. ✅ **编译 libssh2.js**
   - 使用 Emscripten 编译 C++ 代码
   - 生成 WebAssembly 文件
   
5. ✅ **发布到 dist/**
   - 复制编译后的文件到 `dist/` 目录
   - 自动提交并推送（仅主分支）
   - 创建 artifacts 供下载

## 📦 输出文件

构建完成后，`dist/` 目录包含：

```
dist/
├── libssh2.js          # 主要 JavaScript 文件
├── libssh2.wasm        # WebAssembly 二进制文件
├── libssh2-library.js  # 库文件
└── libssh2-wrapper.js  # 包装器文件
```

## 🔧 本地构建

如果需要本地构建：

```bash
# 1. 安装 Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# 2. 构建依赖（参考 deps/BUILD.md）
cd libssh2.js/deps
export EMPORTS=`pwd`
# ... 按照 BUILD.md 构建 zlib, mbedtls, libssh2

# 3. 构建 libssh2.js
cd ..
mkdir -p build && cd build
emcmake cmake ..
emmake make -j4

# 4. 复制到 dist
mkdir -p ../dist
cp libssh2.js libssh2.wasm ../dist/
cp ../js/* ../dist/
```

## 📥 下载构建产物

### 方法 1: GitHub Actions Artifacts

1. 进入仓库的 **Actions** 页面
2. 选择最新的成功构建
3. 下载 **libssh2-js-dist** artifact

### 方法 2: 从 dist 目录

主分支会自动更新 `dist/` 目录，直接 clone 或下载即可：

```bash
git clone https://github.com/yourusername/libssh2.js.git
cd libssh2.js/dist
```

### 方法 3: GitHub Release（针对 Tag）

如果推送了 tag，会自动创建 Release：

```bash
# 创建 tag 并推送
git tag v0.1.0
git push origin v0.1.0

# Release 会包含 libssh2.js 和 libssh2.wasm
```

## ⚙️ 配置选项

### 修改 Emscripten 版本

编辑 `.github/workflows/build-and-deploy.yml`：

```yaml
- name: Setup Emscripten
  uses: mymindstorm/setup-emsdk@v14
  with:
    version: '3.1.50'  # 修改这里
```

### 禁用自动提交

如果不想自动提交到 dist，注释掉工作流中的这部分：

```yaml
- name: Commit and push to dist directory
  if: false  # 添加 false 禁用
```

### 修改触发分支

```yaml
on:
  push:
    branches:
      - main
      - develop  # 添加其他分支
```

## 🐛 常见问题

### Q: 构建失败怎么办？

A: 检查 Actions 日志，常见问题：
- 依赖下载失败（网络问题）
- Emscripten 版本不兼容
- CMake 配置错误

### Q: 缓存没有生效？

A: GitHub Actions 缓存有大小限制（10GB），清理旧缓存：
```bash
# 在 Actions 页面手动删除旧的缓存
```

### Q: dist 目录没有更新？

A: 确保：
1. Push 到的是 main/master 分支
2. 有写入权限
3. commit message 不包含 `[skip ci]`

## 📊 性能优化

当前配置已包含：
- ✅ 依赖库缓存（加速 80%+）
- ✅ 并行编译 `-j$(nproc)`
- ✅ 增量构建检测
- ✅ Release 模式编译

## 📝 维护

### 更新依赖版本

修改工作流中的下载链接：

```yaml
- name: Download dependencies
  run: |
    # 更新版本号
    wget https://www.zlib.net/zlib-1.3.tar.gz
    wget https://github.com/ARMmbed/mbedtls/archive/v3.0.0.zip
    wget https://www.libssh2.org/download/libssh2-1.11.0.tar.gz
```

### 清理构建缓存

定期清理 Actions 缓存以节省空间：
1. Settings → Actions → Caches
2. 删除旧的 `emsdk-cache` 和 `deps` 缓存

---

**最后更新**: 2025年12月31日
