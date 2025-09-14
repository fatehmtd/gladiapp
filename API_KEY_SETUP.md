# API Key Setup Guide

This document explains how to properly configure your Gladia API key for development and production use.

## Quick Start

### Option 1: Environment Variable (Recommended)

1. **Get your API key** from [Gladia](https://gladia.io)
2. **Set the environment variable:**

   **Windows (PowerShell):**
   ```powershell
   $env:GLADIA_API_KEY="your-actual-api-key-here"
   ```

   **Windows (Command Prompt):**
   ```cmd
   set GLADIA_API_KEY=your-actual-api-key-here
   ```

   **Linux/macOS:**
   ```bash
   export GLADIA_API_KEY="your-actual-api-key-here"
   ```

3. **Run your application** - it will automatically use the environment variable

### Option 2: .env File (For Development)

1. **Copy the example file:**
   ```bash
   copy .env.example .env
   ```

2. **Edit `.env`** and replace with your actual API key:
   ```env
   GLADIA_API_KEY=your-actual-api-key-here
   ```
