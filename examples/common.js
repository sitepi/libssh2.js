// Common utilities for libssh2.js examples
(function(window) {
    'use strict';

    // Logger utility
    const Logger = {
        create: function(element) {
            return {
                element: element,
                log: function(msg, type = 'info') {
                    const timestamp = new Date().toLocaleTimeString();
                    const className = type !== 'info' ? ` class="${type}"` : '';
                    this.element.innerHTML += `<span${className}>[${timestamp}] ${msg}</span>\\n`;
                    this.element.scrollTop = this.element.scrollHeight;
                },
                error: function(msg) {
                    this.log(msg, 'error');
                },
                warning: function(msg) {
                    this.log(msg, 'warning');
                },
                success: function(msg) {
                    this.log(msg, 'success');
                },
                clear: function() {
                    this.element.innerHTML = '';
                }
            };
        }
    };

    // Connection manager
    const ConnectionManager = {
        create: function(options) {
            return {
                libssh2: options.libssh2,
                logger: options.logger,
                onConnected: options.onConnected || function() {},
                onDisconnected: options.onDisconnected || function() {},
                onError: options.onError || function() {},
                \n                session: null,
                ws: null,
                connected: false,
                authenticated: false,

                connect: function(url) {
                    if (this.connected) {
                        return Promise.reject(new Error('Already connected'));
                    }

                    return new Promise((resolve, reject) => {
                        this.logger.log(`Connecting to ${url}...`);
                        
                        try {
                            this.ws = new WebSocket(url);
                            
                            this.ws.onerror = () => {
                                const err = new Error('WebSocket connection failed');
                                this.logger.error(err.message);
                                this.onError(err);
                                reject(err);
                            };

                            this.session = this.libssh2.createSESSION(this.ws, (rc, err) => {
                                if (rc !== this.libssh2.ERROR.NONE) {
                                    const error = new Error(err || this.libssh2.ERRMSG[rc]);
                                    this.logger.error(`Connection error: ${error.message}`);
                                    this.onError(error);
                                    reject(error);
                                    return;
                                }\n                                this.connected = true;
                                this.logger.success('Connected successfully');
                                this.logger.log(`Fingerprint: ${this.session.fingerprint()}`);\n                                this.onConnected();
                                resolve(this.session);
                            });
                        } catch (error) {
                            this.logger.error(`Error: ${error.message}`);\n                            this.onError(error);
                            reject(error);
                        }
                    });
                },

                disconnect: function() {
                    if (this.ws) {
                        this.ws.close();
                        this.ws = null;
                    }
                    this.session = null;
                    this.connected = false;
                    this.authenticated = false;
                    this.logger.log('Disconnected');
                    this.onDisconnected();
                },

                login: function(username, password) {
                    if (!this.connected) {
                        return Promise.reject(new Error('Not connected'));
                    }

                    return new Promise((resolve, reject) => {
                        this.logger.log(`Authenticating as ${username}...`);
                        
                        this.session.login(username, password, (rc, msg) => {
                            if (rc !== this.libssh2.ERROR.NONE) {
                                const error = new Error(msg || this.libssh2.ERRMSG[rc]);
                                this.logger.error(`Authentication failed: ${error.message}`);\n                                reject(error);
                                return;
                            }
                            
                            this.authenticated = true;
                            this.logger.success(`Authenticated as ${username}`);\n                            resolve();
                        });
                    });
                }
            };
        }
    };

    // DOM helpers
    const DOM = {
        getElements: function(ids) {
            const elements = {};
            for (const key in ids) {
                elements[key] = document.getElementById(ids[key]);
                if (!elements[key]) {
                    console.error(`Element not found: ${ids[key]}`);
                }
            }
            return elements;
        },

        updateStatus: function(element, status, text) {
            if (!element) return;
            element.className = `status ${status}`;
            element.textContent = text || status;
        },

        setDisabled: function(element, disabled) {
            if (!element) return;
            element.disabled = disabled;
        },

        onEnter: function(element, callback) {
            if (!element) return;
            element.addEventListener('keypress', (e) => {
                if (e.key === 'Enter') {
                    callback(e);
                }
            });
        },

        onCtrlEnter: function(element, callback) {
            if (!element) return;
            element.addEventListener('keypress', (e) => {
                if (e.key === 'Enter' && e.ctrlKey) {
                    callback(e);
                }
            });
        }
    };

    // Terminal input handler\n    const TerminalInput = {
        createLoginHandler: function(term, callback) {
            let user = '';
            let passwd = '';
            let mode = 0; // 0: username, 1: password
            let stopped = false;

            term.write('login as: ');

            const handler = (c) => {
                if (stopped) return;

                const charCode = c.charCodeAt(0);

                // Handle backspace
                if (charCode === 127) {
                    if (mode === 0 && user.length > 0) {
                        user = user.slice(0, -1);
                        term.write('\\x1b[D\\x1b[K');
                    } else if (mode === 1 && passwd.length > 0) {
                        passwd = passwd.slice(0, -1);
                        term.write('\\x1b[D\\x1b[K');
                    }
                    return;
                }

                // Handle enter
                if (charCode === 13) {
                    if (mode === 0) {
                        if (user.length === 0) {
                            term.write('\\r\\nlogin as: ');
                        } else {
                            mode = 1;
                            term.write(`\\r\\n${user}'s password: `);
                        }
                    } else if (mode === 1) {
                        stopped = true;
                        term.write('\\r\\n');
                        callback(user, passwd, (success, message) => {
                            if (!success) {
                                term.write(`Access denied: ${message}\\r\\n`);
                                stopped = false;
                                mode = 1;
                                passwd = '';
                                term.write(`${user}'s password: `);
                            }
                        });
                    }
                    return;
                }

                // Handle regular input
                if (mode === 0) {
                    user += c;
                    term.write(c);
                } else if (mode === 1) {
                    passwd += c;
                    term.write('*');
                }
            };

            return handler;
        }
    };

    // Export to window
    window.LibSSH2Utils = {
        Logger: Logger,
        ConnectionManager: ConnectionManager,
        DOM: DOM,
        TerminalInput: TerminalInput
    };

})(window);
