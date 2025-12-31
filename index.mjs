/**
 * libssh2.js - ES Module Entry Point
 * LibSSH2 Over Stream (WebSocket|WebRTC|Socket) in browsers or Node.js
 * 
 * @module libssh2.js/esm
 * @version 0.1.0
 * @license MIT
 */

import ssh2Loader from './js/libssh2-wrapper.js';

/**
 * Initialize libssh2 library
 * @param {Object} wasmModule - The compiled WASM module
 * @returns {Promise<Object>} Initialized SSH2 library instance
 */
export async function init(wasmModule) {
  if (!wasmModule) {
    throw new Error('WASM module is required');
  }
  
  const ssh2 = await wasmModule();
  ssh2Loader(ssh2);
  
  return {
    ERROR: ssh2Loader.ERROR,
    ERRMSG: ssh2Loader.ERRMSG,
    SFTP: ssh2Loader.SFTP,
    CHANNEL: ssh2Loader.CHANNEL,
    createSession: ssh2Loader.createSESSION,
    version: ssh2.version,
    createSSH: ssh2Loader.createSESSION,
  };
}

// Re-export constants for direct import
export { ERROR, ERRMSG, SFTP, CHANNEL } from './js/libssh2-wrapper.js';

export default init;
