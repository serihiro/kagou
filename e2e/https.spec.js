const { test, expect } = require('@playwright/test');
const { spawn } = require('child_process');
const { exec } = require('child_process');

let server;

test.use({ ignoreHTTPSErrors: true });

test.beforeAll(async () => {
  // build server
  await new Promise((resolve, reject) => {
    const make = spawn('make');
    make.on('close', code => (code === 0 ? resolve() : reject(new Error('make failed'))));
  });
  server = spawn('bash', ['-c', './kagou 8443 test --https']);
  await new Promise(r => setTimeout(r, 1000));
});

test.afterAll(async () => {
  if (server) server.kill('SIGINT');
});

test('serve html over https', async () => {
  await new Promise((resolve, reject) => {
    exec(
      "printf 'GET /test.html HTTP/1.0\r\n\r\n' | openssl s_client -connect localhost:8443 -quiet",
      (error, stdout, stderr) => {
        if (error) return reject(error);
        expect(stdout).toContain('This is Kagou test page');
        resolve();
      }
    );
  });
});
