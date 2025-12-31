# libssh2.js Examples

本目录包含 libssh2.js 的各种使用示例，展示如何通过 WebSocket 在浏览器中实现 SSH2 协议功能。

## 📋 示例列表

### 1. [exec.html](exec.html) - 命令执行
**功能**：连接到 SSH 服务器并执行单个命令
- ✅ WebSocket 连接
- ✅ 用户认证
- ✅ 命令执行
- ✅ 输出捕获

**适用场景**：自动化脚本、远程命令执行、系统监控

---

### 2. [xterm.html](xterm.html) - 交互式终端
**功能**：完整的 SSH shell 终端体验
- ✅ 交互式 shell
- ✅ PTY 支持
- ✅ 终端大小调整
- ✅ 实时输入输出

**适用场景**：Web SSH 客户端、远程管理工具

---

### 3. [scp.html](scp.html) - SFTP 文件管理
**功能**：浏览和管理远程文件系统
- ✅ 目录列表
- ✅ 文件属性查询
- ✅ 文件统计信息
- ✅ SFTP 操作

**适用场景**：Web 文件管理器、文件传输工具

---

### 4. [tcpip.html](tcpip.html) - TCP/IP 转发
**功能**：通过 SSH 隧道进行端口转发
- ✅ Direct TCPIP 通道
- ✅ 端口转发
- ✅ 协议代理
- ✅ HTTP 请求示例

**适用场景**：端口转发、代理服务、内网穿透

---

### 5. [x11.html](x11.html) - X11 转发
**功能**：X11 图形界面转发（实验性）
- ✅ X11 转发通道
- ✅ X Server 集成
- ✅ 图形应用支持

**适用场景**：远程桌面、图形应用

---

## 🚀 快速开始

### 前提条件

1. **构建 libssh2.js**
   ```bash
   cd ..
   mkdir -p build && cd build
   emcmake cmake ..
   emmake make
   ```

2. **启动 WebSocket 代理服务器**
   
   需要一个 WebSocket 到 TCP 的代理服务器，例如使用 [websockify](https://github.com/novnc/websockify)：
   
   ```bash
   # 安装 websockify
   pip install websockify
   
   # 启动代理（将 WebSocket 7681 端口代理到 SSH 22 端口）
   websockify 7681 localhost:22
   ```

3. **启动 Web 服务器**
   ```bash
   # Python 3
   python -m http.server 8000
   
   # 或使用 Node.js
   npx http-server -p 8000
   ```

4. **打开浏览器**
   访问 `http://localhost:8000/examples/xterm.html`

---

## 🛠️ 配置说明

### WebSocket URL 格式

```
ws://[host]:[port]/[path]
```

示例：
- `ws://127.0.0.1:7681/ssh` - 本地代理
- `ws://192.168.1.100:7681/xxx` - 局域网代理
- `wss://example.com/ssh` - 安全 WebSocket（推荐生产环境）

### 连接参数

所有示例都支持以下参数：
- **URL**: WebSocket 代理服务器地址
- **Username**: SSH 用户名
- **Password**: SSH 密码（建议使用密钥认证）

---

## 📖 代码示例

### 基本连接

```javascript
// 1. 加载 libssh2
let libssh2;
ssh2Loader().then((wasm) => {
    libssh2 = wasm;
    libssh2.init(0);
});

// 2. 创建 WebSocket 连接
const ws = new WebSocket('ws://127.0.0.1:7681/ssh');

// 3. 创建 SSH 会话
const session = libssh2.createSESSION(ws, (rc, err) => {
    if (rc !== libssh2.ERROR.NONE) {
        console.error('Connection failed:', err);
        return;
    }
    console.log('Connected!');
    console.log('Fingerprint:', session.fingerprint());
});

// 4. 用户认证
session.login(username, password, (rc, msg) => {
    if (rc !== libssh2.ERROR.NONE) {
        console.error('Login failed:', msg);
        return;
    }
    console.log('Authenticated!');
});
```

### 执行命令

```javascript
session.CHANNEL((rc, channel) => {
    if (rc !== libssh2.ERROR.NONE) return;
    
    // 接收输出
    channel.onmessage((rc, msg) => {
        console.log(msg);
    });
    
    // 执行命令
    channel.exec('ls -la', (rc, msg) => {
        if (rc === libssh2.ERROR.NONE) {
            console.log('Command executed');
        }
    });
});
```

### Shell 交互

```javascript
session.CHANNEL((rc, channel) => {
    if (rc !== libssh2.ERROR.NONE) return;
    
    // 接收输出
    channel.onmessage((rc, msg) => {
        terminal.write(msg);
    });
    
    // 启动 shell
    channel.shell((rc, msg) => {
        if (rc === libssh2.ERROR.NONE) {
            // 设置终端大小
            channel.pty_size(cols, rows, (rc, msg) => {
                console.log('PTY size set');
            });
            
            // 发送输入
            terminal.onData((data) => {
                channel.send(data);
            });
        }
    });
});
```

### SFTP 操作

```javascript
session.SFTP((rc, sftp) => {
    if (rc !== libssh2.ERROR.NONE) return;
    
    // 获取当前目录
    sftp.realpath('.', (rc, path) => {
        console.log('Current dir:', path);
    });
    
    // 列出目录
    sftp.opendir('/home/user', (rc, handle) => {
        if (rc !== libssh2.ERROR.NONE) return;
        
        handle.readdir((rc, filename) => {
            if (rc === libssh2.ERROR.NONE && filename) {
                console.log('File:', filename);
            }
        });
    });
    
    // 获取文件属性
    sftp.stat('/path/to/file', (rc, attrs) => {
        if (rc === libssh2.ERROR.NONE) {
            console.log('Size:', attrs.filesize);
            console.log('Permissions:', attrs.perm);
            console.log('Modified:', new Date(attrs.mtime * 1000));
        }
    });
});
```

### TCP/IP 转发

```javascript
const targetHost = '192.168.1.100';
const targetPort = 80;

session.TCPIP(targetHost, targetPort, (rc, channel) => {
    if (rc !== libssh2.ERROR.NONE) return;
    
    // 接收数据
    channel.onmessage((rc, msg) => {
        console.log('Received:', msg);
    });
    
    // 发送 HTTP 请求
    channel.send('GET / HTTP/1.1\\r\\n' +
                 `Host: ${targetHost}\\r\\n` +
                 '\\r\\n');
});
```

---

## 🎨 样式定制

示例使用了共享的样式文件 [common.css](common.css)，包含：
- 响应式布局
- 深色终端主题
- 状态指示器
- 表单样式
- 按钮样式

你可以通过覆盖 CSS 变量来定制主题：

```css
:root {
    --primary-color: #5DA5D5;
    --terminal-bg: #1e1e1e;
    --terminal-text: #00ff00;
}
```

---

## 🔒 安全建议

### 生产环境注意事项

1. **使用 WSS (WebSocket Secure)**
   ```javascript
   const ws = new WebSocket('wss://secure.example.com/ssh');
   ```

2. **不要在客户端硬编码密码**
   - 使用密钥认证
   - 实现安全的凭证存储
   - 考虑使用 OAuth/JWT

3. **验证服务器指纹**
   ```javascript
   const knownFingerprint = 'expected_fingerprint';
   if (session.fingerprint() !== knownFingerprint) {
       console.error('Fingerprint mismatch!');
       // 断开连接
   }
   ```

4. **实现会话超时**
   ```javascript
   let sessionTimeout = setTimeout(() => {
       session.disconnect();
       console.log('Session timed out');
   }, 30 * 60 * 1000); // 30 minutes
   ```

5. **输入验证和清理**
   ```javascript
   function sanitizeInput(input) {
       return input.replace(/[<>&\"']/g, '');
   }
   ```

---

## 🐛 故障排查

### 常见问题

**Q: WebSocket 连接失败**
```
A: 检查：
   1. WebSocket 代理服务器是否运行
   2. 端口是否正确
   3. 防火墙设置
   4. 浏览器控制台错误信息
```

**Q: 认证失败**
```
A: 确认：
   1. 用户名和密码正确
   2. SSH 服务器允许密码认证
   3. 用户有登录权限
```

**Q: 终端显示乱码**
```
A: 设置正确的字符编码：
   terminal.options.charset = 'UTF-8';
```

**Q: 命令执行没有输出**
```
A: 添加延迟等待输出：
   setTimeout(() => {
       // 读取输出
   }, 500);
```

### 调试技巧

1. **启用浏览器控制台**
   ```javascript
   console.log('Debug:', rc, msg);
   ```

2. **监控 WebSocket 流量**
   - Chrome DevTools → Network → WS
   - 查看发送/接收的数据帧

3. **检查错误代码**
   ```javascript
   if (rc !== libssh2.ERROR.NONE) {
       console.error('Error:', libssh2.ERRMSG[rc]);
   }
   ```

---

## 📚 更多资源

- [libssh2.js GitHub](https://github.com/yourusername/libssh2.js)
- [libssh2 官方文档](https://www.libssh2.org/)
- [xterm.js 文档](https://xtermjs.org/)
- [WebSocket API](https://developer.mozilla.org/en-US/docs/Web/API/WebSocket)

---

## 📝 许可证

MIT License - 详见项目根目录 LICENSE 文件

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

改进建议：
- 新的示例场景
- 性能优化
- UI/UX 改进
- 文档完善

---

**最后更新**: 2025年12月31日
