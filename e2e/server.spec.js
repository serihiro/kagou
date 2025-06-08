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

test('handle 404 and then serve existing file correctly', async ({ browser }) => {
  // First, access a non-existent file and expect 404
  const page1 = await browser.newPage();
  const response404 = await page1.goto('http://localhost:8000/nonexistent.html');
  expect(response404.status()).toBe(404);
  await expect(page1.locator('h1')).toHaveText('Not Found');
  await expect(page1.locator('p')).toHaveText('The requested URL is Not Found');
  await page1.close();

  // Then, access an existing file and expect 200 with correct content
  const page2 = await browser.newPage();
  const response200 = await page2.goto('http://localhost:8000/test.html');
  expect(response200.status()).toBe(200);
  await expect(page2.locator('h1')).toHaveText('This is Kagou test page');
  const img = page2.locator('img');
  await expect(img).toBeVisible();
  await expect(await img.evaluate(el => el.naturalWidth)).toBeGreaterThan(0);
  await page2.close();
});

test('download JSON file', async ({ page }) => {
  // Navigate to the JSON file directly
  const response = await page.goto('http://localhost:8000/test.json');
  
  // Check response status and content type
  expect(response.status()).toBe(200);
  expect(response.headers()['content-type']).toBe('application/json');
  
  // Check the content
  const jsonContent = await response.json();
  expect(jsonContent).toHaveProperty('Naoya');
  expect(jsonContent.Naoya.Like).toBe('SHIROBAKO');
  expect(jsonContent).toHaveProperty('Tatsuhiko');
  expect(jsonContent.Tatsuhiko.Like).toBe('visa');
});

test('download CSV file', async ({ page }) => {
  // First navigate to a page
  await page.goto('http://localhost:8000/test.html');
  
  // Use fetch to get the CSV file content
  const csvData = await page.evaluate(async () => {
    const response = await fetch('http://localhost:8000/test.csv');
    return {
      status: response.status,
      contentType: response.headers.get('content-type'),
      text: await response.text()
    };
  });
  
  // Check response status and content type
  expect(csvData.status).toBe(200);
  expect(csvData.contentType).toBe('text/csv');
  
  // Check the content
  expect(csvData.text).toContain('"Name","Like"');
  expect(csvData.text).toContain('"Naoya","Anime"');
  expect(csvData.text).toContain('"Tatsuhiko","Perfume"');
});
