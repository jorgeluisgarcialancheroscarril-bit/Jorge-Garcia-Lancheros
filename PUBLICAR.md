# 🚀 Cómo publicar Fertilízate en GitHub (repositorio + sitio web)

Al terminar tendrás:
- el repositorio con el README interactivo (Mermaid, alertas, desplegables), y
- una **página web interactiva** (juegos, quiz y calculadora; la documentación completa queda en `/documentacion.html`) en `https://TU-USUARIO.github.io/fertilizate/` generada automáticamente desde el README.

## 1. Personaliza antes de subir
- [ ] Reemplaza `TU-USUARIO` por tu usuario u organización de GitHub en: `README.md`, `CITATION.cff` y (si lo cambias) el nombre del repo.
- [ ] Completa nombres en `CITATION.cff`, `LICENSE` (titular) y `CODE_OF_CONDUCT.md` (correo de contacto).
- [ ] Sube tus fotos a `docs/imagenes/` y enlázalas en el README con `![texto](docs/imagenes/foto.jpg)`.
- [ ] Rellena los `___` del README con tus datos reales.

## 2. Crear el repositorio y subir los archivos
1. En GitHub: **New repository** → nombre `fertilizate` → público → **sin** README, .gitignore ni licencia (ya vienen incluidos).
2. En la carpeta del proyecto:

```bash
git init
git add .
git commit -m "feat: primera versión de Fertilízate"
git branch -M main
git remote add origin https://github.com/TU-USUARIO/fertilizate.git
git push -u origin main
```

> Si prefieres no usar la terminal: en GitHub, **Add file → Upload files** y arrastra el contenido de la carpeta (incluida `.github/`, que es una carpeta oculta).

## 3. Activar la página web (GitHub Pages)
1. Repositorio → **Settings → Pages**.
2. En **Build and deployment → Source** elige **GitHub Actions**.
3. Ve a la pestaña **Actions**: el workflow *Publicar sitio (GitHub Pages)* se ejecuta solo con cada `push` a `main` (o ejecútalo con **Run workflow**).
4. Cuando termine en verde, tu sitio estará en `https://TU-USUARIO.github.io/fertilizate/`.

## 4. Comprobar que todo funciona
- [ ] El README se ve bien en GitHub: los 3 diagramas Mermaid se dibujan, las alertas tienen color y los `<details>` se despliegan.
- [ ] Las anclas de la tabla de contenidos llevan a cada sección.
- [ ] La pestaña **Actions** muestra el workflow **CI** en verde (compila el firmware para Arduino UNO y ejecuta los scripts).
- [ ] El sitio web carga y los enlaces a archivos abren el repositorio.

## 5. Presentación del repositorio
- [ ] **About** (engranaje junto a la descripción): descripción corta, enlace al sitio web y temas (*topics*): `compostaje`, `arduino`, `residuos-organicos`, `economia-circular`, `open-hardware`, `educacion`.
- [ ] Sube una imagen de vista previa en **Settings → General → Social preview** (1280×640 px).
- [ ] Crea un *release* (`v1.0`) cuando termines la entrega académica; así queda una versión congelada.
- [ ] Activa **Discussions** si quieres que la comunidad pregunte sin abrir *issues*.

## Vista previa local del sitio (opcional)
Desde la carpeta del proyecto, copia `docs/index.html` junto a `README.md` y sirve la carpeta:

```bash
mkdir -p _site && cp docs/index.html README.md _site/
cd _site && python3 -m http.server 8000
# abre http://localhost:8000
```

## Problemas frecuentes
| Síntoma | Solución |
|---|---|
| El sitio da error 404 | Revisa que en *Settings → Pages* el origen sea **GitHub Actions** y que el workflow haya terminado en verde |
| El workflow de Pages falla en `deploy` | Primero activa Pages (paso 3) y vuelve a ejecutar el workflow |
| El job *Compilar firmware* falla | Abre el log: suele ser un nombre de librería o un error de sintaxis que se corrige en `firmware/` |
| Un diagrama Mermaid muestra error | Revisa la sintaxis del bloque ` ```mermaid ` indicado en el mensaje de error |
| Los badges de CI/Pages no cargan | Reemplaza `TU-USUARIO` y el nombre del repositorio en las URLs |
