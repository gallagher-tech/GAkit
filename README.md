# ga::kit

`ga::kit` is G&A's in-house C++ application framework for building interactive experiences.  
We use it to create anything from 2D UI prototypes to high-performance 3D immersive installations.

We designed it from scratch to be:

- _fast_ -  
  Our scenegraph lets you intuitively craft complex 3D scenes without sacrificing performance.

- _flexible_ -  
  Every app is different. We use modular components, templated classes, and helpful utilities to support iterative, agile development.

- _familiar_ -  
  As creative coders, we love the clarity and efficacy of existing toolkits like openFrameworks and Cinder.  
  That's why we designed `ga::kit` to work as an oF addon - and a Cinder Block is on the roadmap.  
  Or bring your own rendering engine - we strive to be framework agnostic.

## How To Use

### ofxGANative openFrameworks addon

Currently, the simplest way to use `ga::kit` is as an openFrameworks addon. _Todo: ofxGANative repo link_

Simply `git clone` the addon repo inside of your `openFrameworks/addons/` directory, then use the oF Project Generator to generate a project with the addon selected.

In this case, openFrameworks provides the windowing and rendering system (`glfw / openGL`), and automatically includes the necessary `nlohmann::json` and `glm` libraries.

### Bring Your Own Backend

We aim to make `ga::kit` "framework agnostic". This means that we would like it to play nicely with other C++ creative coding frameworks like Cinder - or it could be used on its own by including a few supporting libraries.

The only requirements beyond C++11 are including the `glm` and `nlohmann::json` libraries, and adding an openGL windowing library, like `glfw`.

We could use your help:

- creating a Cinder block. _Todo: Cinder block repo link_
- creating a "framework-free" example project, showing how to include `glm`, `nlohman::json` and `glfw`

## Features

- Hierarchical 3D **Scenegraph** with `Scene`, `Node`, and `Component` classes
- Innovative **tween animation system** with `Tween`, `Timeline` and easing functions
- `Timer` and `Timeout` classes for triggering timed callbacks
- Universal app **asset management** with templated `ResourceCache` class
- `KeyEvent`, `MouseEvent` and `TouchEvent` handling
- **Layout** utitilies to automatically align and scale to fit
- Math utilities built on [**glm**](https://github.com/g-truc/glm)
- JSON serialization, using [**nlohmann::json**](https://github.com/nlohmann/json)
- Signal-based event system, using [**sigslot**](https://github.com/palacaze/sigslot) (included)
- Universal Unique ID system, using [**crossguid**](https://github.com/graeme-hill/crossguid) (included)

## Todo

- GLSL `Shader` class
- `Material` (shader + textures + parameters) component
- `Mesh` VBO component
- Cinder block
- Framework-free example using GLFW

## Requirements

- C++11
- OpenGL 3+
- [**glm**](https://github.com/g-truc/glm)
- [**nlohmann::json**](https://github.com/nlohmann/json)
