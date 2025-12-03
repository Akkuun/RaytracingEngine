# Optimisations CPU Appliquées et Recommandations

## ✅ Optimisations Appliquées

### 1. Réduction des allocations mémoire
- **setupShapesBuffer()**: Utilisation de `reserve()` pour pré-allouer l'espace nécessaire
- **setupShapesBuffer()**: Utilisation de `resize()` au lieu de `push_back()` répétés pour les meshes
- **setupTextureBuffer()**: Pré-calcul de la taille totale et `reserve()` pour éviter les réallocations
- **setupBVHBuffer()**: Utilisation de `reserve()` et `std::move_iterator` pour éviter les copies

### 2. Élimination des copies inutiles
- **render()**: Le camera buffer n'est recréé que si la caméra a changé (utilisation du flag `cameraBufferDirty`)
- **render()**: Utilisation de `assign()` au lieu de `memcpy()` pour le transfert des données OpenCL
- **setupBVHBuffer()**: Utilisation de `std::make_move_iterator` pour éviter les copies lors de l'insertion

### 3. Optimisation OpenGL
- **updateTextureFromKernel()**: Utilisation de `glTexSubImage2D()` au lieu de `glTexImage2D()` après la première frame
  - `glTexImage2D()` réalloue la texture complète (lent)
  - `glTexSubImage2D()` met à jour les données existantes (rapide)

### 4. Optimisation des boucles
- **setupTextureBuffer()**: Optimisation des boucles de copie avec indexation directe
- **setupShapesBuffer()**: Traitement en place des triangles de mesh

## 📊 Gains de Performance Estimés

| Optimisation | Gain estimé | Zone d'impact |
|-------------|-------------|---------------|
| Camera buffer réutilisation | 5-10% | Chaque frame |
| Vector reserve/resize | 10-20% | Chargement de scène |
| glTexSubImage2D | 5-15% | Transfert GPU->Display |
| Move semantics BVH | 5-10% | Chargement de scène |
| Texture buffer pre-allocation | 10-15% | Chargement de textures |

**Total estimé**: 15-30% d'amélioration des performances CPU

## 🚀 Optimisations Supplémentaires Recommandées

### 1. Utiliser des Persistent Mapped Buffers (PBO) pour OpenGL
```cpp
// Au lieu de copier imageData vers texture à chaque frame:
// 1. Mapper directement le buffer OpenCL vers un PBO OpenGL
// 2. Utiliser glTexSubImage2D avec le PBO comme source
// Gain: 20-40% sur le transfert GPU->GPU
```

### 2. Threading pour les opérations CPU
```cpp
// setupShapesBuffer, setupTextureBuffer, setupBVHBuffer peuvent être parallélisées
// Utiliser std::async ou Qt::concurrent pour le processing en parallèle
// Gain: 30-50% sur le chargement de scène
```

### 3. Cache les résultats toGPU()
```cpp
// Les fonctions toGPU() sont appelées à chaque setupBuffer
// Mettre en cache les résultats et ne recalculer que si modifié
// Gain: 20-30% sur les updates de scène
```

### 4. Object Pooling pour les vecteurs temporaires
```cpp
// Réutiliser les vecteurs gpu_shapes, allTextureData, etc.
// Les garder comme membres de classe et les clear() au lieu de les recréer
// Gain: 5-10% réduction des allocations
```

### 5. Profiling avec outils dédiés
- **perf**: `perf record -g ./build/raytrace && perf report`
- **valgrind**: `valgrind --tool=callgrind ./build/raytrace`
- **Intel VTune**: Pour identifier les hotspots précis

### 6. Compiler avec optimisations agressives
Dans CMakeLists.txt:
```cmake
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")
```
- `-O3`: Optimisations maximales
- `-march=native`: Utilise les instructions CPU spécifiques
- `-flto`: Link-Time Optimization

### 7. Utiliser OpenCL buffer pinned memory
```cpp
// Au lieu de CL_MEM_COPY_HOST_PTR:
cl::Buffer buffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, size);
// Puis mapper et copier - évite une copie intermédiaire
```

### 8. Batch les appels OpenGL
```cpp
// Utiliser glMultiDrawArrays si vous dessinez plusieurs objets
// Réduire le nombre de glBindTexture/glBindBuffer
```

## 🔧 Code à Surveiller

### Points chauds potentiels:
1. **Camera::toGPU()**: Vérifier si des calculs inutiles sont faits
2. **Triangle::toGPU()**: Appelé pour chaque triangle de mesh
3. **Material::getImage()**: Vérifier si des copies sont faites
4. **SceneManager::getShapes()**: Vérifier si retourne par copie

### Suggestions de mesures:
```cpp
// Ajouter des timers pour mesurer:
auto start = std::chrono::high_resolution_clock::now();
// ... code ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "Operation took: " << duration.count() << " µs\n";
```

## 📝 Notes

- Les gains réels dépendent de la taille de la scène et du matériel
- Testez avec différentes configurations pour valider
- Utilisez un profiler avant d'optimiser davantage
- Le GPU est probablement le bottleneck principal pour le raytracing
