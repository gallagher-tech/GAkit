# GA::kit
  
| [GA::kit Repo](https://github.com/gallagher-tech/GAkit/) | [ofxGAkit Repo](https://github.com/gallagher-tech/ofxGAkit) | [Documentation](https://gallagher-tech.github.io/GAkit-doc/) |
  
`GA::kit` is G&A's in-house C++ application framework for building interactive experiences.  
We use it to create anything from 2D UI prototypes to high-performance 3D immersive installations.

We designed it from scratch to be:

- _fast_ -  
  Our scenegraph lets you intuitively craft complex 3D scenes without sacrificing performance.

- _flexible_ -  
  Every app is different. We use modular components, templated classes, and helpful utilities to support iterative, agile development.

- _familiar_ -  
  As creative coders, we love the clarity and efficacy of existing toolkits like openFrameworks and Cinder.  
  That's why we designed `GA::kit` to work as an oF addon - and a Cinder Block is on the roadmap.  
  Or bring your own rendering engine - we strive to be framework agnostic.

## How To Use

### ofxGAkit openFrameworks addon

Currently, the simplest way to use `GA::kit` is through [ofxGAkit](https://github.com/gallagher-tech/ofxGAkit) openFrameworks addon.  

Simply `git clone --recursive` the addon repo inside of your `openFrameworks/addons/` directory, then use the oF Project Generator to generate a project with the addon selected.

The addon works for both Windows and Mac. Full setup instructions can be found [here](https://github.com/gallagher-tech/ofxGAkit).

In this case, openFrameworks provides the windowing and rendering system (`glfw / openGL`), and automatically includes the necessary `nlohmann::json` and `glm` libraries.

### Bring Your Own Backend

We aim to make `GA::kit` "framework agnostic". This means that we would like it to play nicely with other C++ creative coding frameworks like Cinder - or it could be used on its own by including a few supporting libraries.

The only requirements beyond C++14 are including the `glm` and `nlohmann::json` libraries, and adding an openGL windowing library, like `glfw`.

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

- C++14
- OpenGL 3+
- [**glm**](https://github.com/g-truc/glm)
- [**nlohmann::json**](https://github.com/nlohmann/json)

## About G&A  
We are the industry’s best strategic problem solvers. A team of boundless creative individuals. As experts in our own craft, relentless curiosity fuels our passion for working with the best partners to build community experiences led by their profound stories. Bring your story and become part of ours.  

Visit G&A [website](https://gallagherdesign.com/)  
