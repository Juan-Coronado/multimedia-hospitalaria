# Multimedia hospitalaria

Prototipo de organización y búsqueda de archivos multimedia mediante estructuras de datos propias.

[Ficha y evidencia en el portafolio](https://juan-coronado.github.io/projects/multimedia-hospitalaria/)

| Periodo original | Dificultad académica | Estado de esta versión |
| --- | --- | --- |
| Junio de 2026 | Avanzada | Prototipo comprobado |

## Funcionalidades

- Registro y consulta de pacientes ficticios.
- Índices hash, trie y árbol rojo-negro.
- Copia de multimedia y persistencia del estado.

## Ejecución

Windows con las herramientas C++ de Visual Studio. Abrir Developer PowerShell en la raíz del repositorio. Usar exclusivamente datos y archivos ficticios.

```powershell
cl /nologo /std:c++17 /EHsc /utf-8 /D_HAS_STD_BYTE=0 programa/SistemaHospitalarioMultimedia/src/*.cpp shell32.lib /Fe:hospital.exe
.\hospital.exe
```

## Comprobación disponible

Compilado con MSVC C++17. Pruebas con 300 pacientes ficticios, invariantes del árbol, PNG real, hash, trie y persistencia.

Ejecutar desde la raíz del repositorio. Ejecutar en una copia de pruebas, porque se guardan archivos locales.

```powershell
cl /nologo /std:c++17 /EHsc /utf-8 /D_HAS_STD_BYTE=0 tests/estructuras.cpp programa/SistemaHospitalarioMultimedia/src/BusinessLogic.cpp programa/SistemaHospitalarioMultimedia/src/FileSystem.cpp programa/SistemaHospitalarioMultimedia/src/MotorAlgoritmico.cpp shell32.lib /Fe:pruebas.exe
.\pruebas.exe "RUTA_A_UN_PNG_DE_PRUEBA"
```

[Registro de ejecución](evidencia/ejecucion.txt)

[Registro de pruebas](evidencia/pruebas.txt)

Las comprobaciones anteriores corresponden a la recuperación de octubre de 2026; esta publicación conserva sus registros.

## Aprendizaje

- Combinar estructuras de datos e índices relacionados.
- Gestionar memoria y conservar la integridad al guardar o eliminar registros.

## Revisión para el portafolio

- Eliminada la reserva inicial excesiva de memoria.
- Corregidos índices al eliminar, lectura de archivos corruptos y guardado atómico.

## Alcance

Prototipo educativo: usar solo datos ficticios. La ofuscación original no es cifrado seguro y los binarios no se cifran. No se hicieron pruebas con millones de registros ni una validación para uso clínico.

## Contexto académico y procedencia

Este repositorio conserva el alcance académico y los créditos disponibles en el material original. La recuperación, las correcciones y las reconstrucciones de octubre de 2026 se realizaron con asistencia de Codex/IA y se distinguen del trabajo original. La contribución individual de cada integrante no está desglosada aquí.
