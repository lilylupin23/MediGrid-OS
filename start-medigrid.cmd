@echo off
setlocal
cd /d "%~dp0"
where node >nul 2>nul
if errorlevel 1 (
  echo Node.js was not found. Install Node.js 22 or newer from https://nodejs.org
  pause
  exit /b 1
)
echo Starting MediGrid OS. Open http://127.0.0.1:3000 in your browser.
echo Keep this window open. Press Ctrl+C to stop the server.
node web/server.mjs
if errorlevel 1 pause
