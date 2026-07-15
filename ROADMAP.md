
title: "ImGui Node Editor Roadmap"
description: "Mejoras planificadas y bugs conocidos"
created: "2026-07-15"
---

# 🗺️ Roadmap

## 🐛 Bugs conocidos
- [ ] RowNode: SpaceBetween dentro de Column con Indent calcula mal el ancho disponible
- [ ] ComponentNode: hover preview no escala correctamente nodos con muchos pines
- [ ] Node self-connection: está bloqueado en HandleLinkDrop pero TryAutoConnect podría permitirlo
- [ ] InputTextNode font scale: `SetWindowFontScale` gotea cuando no se restaura

## 🎯 Mejoras pendientes

### Canvas
- [x] Zoom suave (interpolación)
- [x] Sombras en nodos
- [x] Pin tooltips al hover
- [ ] Grid adaptativo (líneas más separadas al alejar zoom)
- [ ] Performance con 100+ nodos (frustum culling)

### ThemeNode
- [ ] ItemSpacing como campo del tema
- [ ] WindowBorderSize como estilo de ventana

### Nodos
- [ ] ButtonNode: pin Anim (para animar hover/click)
- [ ] MenuItemNode: pin Anim (para animar check)
- [ ] InputIntNode: pin Anim (para animar cambio de valor)
- [ ] SeparatorNode: FindNodeTheme ya añadido, probar que funciona
- [ ] TreeNode: verificar que los colores del tema se aplican correctamente

### Sistema de archivos
- [x] Regiones serializadas en .ng
- [ ] Guardar ventanas acopladas (docking layout)
- [ ] Exportar a código C++ (generar ImGui::Button, etc.)

### Templates
- [ ] Login template completo con animación, componente y regiones
- [ ] Dashboard template (gráficos, tabs, tablas)
- [ ] Settings window template

### Release
- [x] v0.1 publicada
- [ ] v0.2 con nodos de InputText/ComboBox/ColorEdit funcionales al 100%
- [ ] CI/CD: GitHub Actions para build automático
- [ ] Probar en Linux (GCC/Clang) y macOS
