# Figma2UMG

**Import your Figma designs directly into Unreal Engine UMG** — turning your Figma frames into usable UMG widgets automatically.

---

## 🚀 Overview

**Figma2UMG** is an Unreal Engine plugin developed by Buvi Games that brings your Figma UI designs into Unreal with just a few clicks. It’s designed to accelerate UI prototyping and align design and development workflows by translating visual layouts into native UMG components.

Originally released on the Epic Marketplace, this plugin is now **open source and free to use** — with continued support for existing users.

---

## ✨ Features

- 📦 Convert Figma frames into UMG widgets
- 🧩 Preserves hierarchy and nesting of UI elements
- 🔤 Text and shape layers supported
- 🎨 Basic styling: fills, positions, sizes
- 📁 Generates Blueprint UserWidgets for rapid iteration

---

## 📥 Installation

### 🔹 Option 1: From Source (GitHub)
1. Clone or download this repository into your Unreal project’s `Plugins/` folder.
2. Launch Unreal Engine.
3. Enable the **Figma2UMG** plugin from the **Plugins** browser.
4. Restart Unreal if prompted.

### 🔹 Option 2: From FAB
Install the plugin directly from FAB:
👉 [https://www.fab.com/listings/0e0d4d1f-702f-4b3b-96c1-01c0fcac7823](https://www.fab.com/listings/0e0d4d1f-702f-4b3b-96c1-01c0fcac7823)

---

## 🎮 Usage

1. In Unreal, open the **Import Figma file** from the Content Browser's Context Menu
2. Enter your **Figma Personal Access Token** and **File ID**. - https://www.figma.com/developers/api#access-tokens
3. (Optional) You can add IDs to import only elements of your file.
4. Set your desired import path in the Content Browser (e.g., `/Game/UI/Figma/`).
5. Click **Import**.

Once done, the plugin will generate UMG widgets representing your Figma layout, ready to be used or customized in Blueprint.

---

## 📚 Resources

- 📄 **Documentation**: [https://www.buvi.games/figma2umg](https://www.buvi.games/figma2umg)
- 🔌 **Figma Plugin**: [https://www.figma.com/community/plugin/1368487806996965174/figma2umg-unreal-importer](https://www.figma.com/community/plugin/1368487806996965174/figma2umg-unreal-importer)
- 💬 **Support**: [figma2umg@buvi.games](mailto:figma2umg@buvi.games)

---

## 🐞 Bug Reports

Please send an email to [figma2umg@buvi.games](mailto:figma2umg@buvi.games) and include:
- A **Figma file** for testing  
- If the issue is related to layout rendering, include a screenshot or UMG showing the **expected result**

This will help reproduce and fix the problem more efficiently.

---

## 📌 Roadmap & Community

Planned improvements include:

- Multi-frame support
- Better layout handling (e.g., Horizontal/Vertical/Grid Boxes)
- Font and style syncing
- Text formatting (justification, wrapping)
- Outline and gradient support

**Want to help?** Contributions are welcome — feel free to submit issues, feature requests, or pull requests.

---

## 💬 Continued Support

If you purchased the plugin via the E
