const { test, expect } = require('@playwright/test');
const { spawn } = require('child_process');
const path = require('path');

let server;

test.beforeAll(async () => {
  // build server
  await new Promise((resolve, reject) => {
    const make = spawn('make');
    make.on('close', code => (code === 0 ? resolve() : reject(new Error('make failed'))));
  });
  server = spawn('./kagou', ['8000', 'test']);
  // wait a bit for server to start
  await new Promise(r => setTimeout(r, 1000));
});

test.afterAll(async () => {
  if (server) server.kill('SIGINT');
});

test('serve html with image', async ({ page }) => {
  await page.goto('http://localhost:8000/test.html');
  await expect(page.locator('h1')).toHaveText('This is Kagou test page');
  const img = page.locator('img');
  await expect(img).toBeVisible();
  await expect(await img.evaluate(el => el.naturalWidth)).toBeGreaterThan(0);
});
