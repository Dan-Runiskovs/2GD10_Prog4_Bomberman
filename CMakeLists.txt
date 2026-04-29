cmake_minimum_required(VERSION 3.26)
project(Minigin)

# ---------------------------
# Platform
# ---------------------------
if(EMSCRIPTEN)
  message("Using Emscripten!")
  set(TARGET_NAME bomberman_web)
  set(TARGET_PARAM "")
else()
  set(TARGET_NAME bomberman)
  set(TARGET_PARAM WIN32)
endif()

# ---------------------------
# Dependencies
# ---------------------------
include(FetchContent)

# glm
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
  )
  FetchContent_MakeAvailable(glm)
endif()

# SDL3
find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND)
  FetchContent_Declare(
    SDL3
    URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
  )
  FetchContent_MakeAvailable(SDL3)
  list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}")
  find_package(SDL3 CONFIG REQUIRED)
endif()

# SDL_ttf
find_package(SDL3_ttf CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  FetchContent_Declare(
    SDL3_ttf
    URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
  )
  FetchContent_MakeAvailable(SDL3_ttf)
  list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}")
  find_package(SDL3_ttf CONFIG REQUIRED)
endif()

# SDL_mixer
find_package(SDL3_mixer 3.0.0 CONFIG QUIET)
if(NOT SDL3_mixer_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3_mixer
      URL https://www.libsdl.org/projects/SDL_mixer/release/SDL3_mixer-devel-3.2.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_mixer)
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_mixer_SOURCE_DIR}")
    find_package(SDL3_mixer CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_mixer
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
      GIT_TAG release-3.2.0
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    set(SDL3MIXER_INSTALL OFF)

    if(EMSCRIPTEN)
      set(SDL3MIXER_VENDORED ON)
      set(SDL3MIXER_OGG ON)
      set(SDL3MIXER_MP3 ON)
    endif()

    FetchContent_MakeAvailable(SDL3_mixer)
  endif()
endif()

# ---------------------------
# ENGINE (STATIC LIB)
# ---------------------------
add_library(minigin_lib STATIC
  Minigin/Bindings.cpp
  Minigin/Commands.cpp
  Minigin/ComponentBase.cpp
  Minigin/Controller.cpp
  Minigin/Font.cpp
  Minigin/Game.cpp
  Minigin/GameObject.cpp
  Minigin/InputManager.cpp
  Minigin/Minigin.cpp
  Minigin/Renderer.cpp
  Minigin/ResourceManager.cpp
  Minigin/Scene.cpp
  Minigin/SceneManager.cpp
  Minigin/ServiceLocator.cpp
  Minigin/SoundSystem.cpp
  Minigin/Texture2D.cpp
  Minigin/Timer.cpp
  Minigin/TransformComponent.cpp
  Minigin/RenderComponent.cpp
  Minigin/PhysicsComponent.cpp
  Minigin/HealthComponent.cpp
  Minigin/Subject.cpp
  Minigin/ScoreManager.cpp
)

target_compile_features(minigin_lib PUBLIC cxx_std_20)

target_include_directories(minigin_lib PUBLIC
  ${CMAKE_SOURCE_DIR}/Minigin
)

target_link_libraries(minigin_lib PUBLIC
  SDL3::SDL3
  SDL3_ttf::SDL3_ttf
  SDL3_mixer::SDL3_mixer
  glm::glm
)

# ---------------------------
# GAME (EXECUTABLE)
# ---------------------------
add_executable(${TARGET_NAME} ${TARGET_PARAM}
  Game/Main.cpp
  Game/Bomberman.cpp

  GameComponents/AnimationComponent.cpp
  GameComponents/FPSComponent.cpp
  GameComponents/OrbitComponent.cpp
  GameComponents/TextComponent.cpp

  Extras/Achievements.cpp
  Extras/StatsAndAchievements.cpp
  #Extras/ThrashTheCache.cpp
)

target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)

target_include_directories(${TARGET_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/Game
  ${CMAKE_SOURCE_DIR}/GameComponents
  ${CMAKE_SOURCE_DIR}/Extras
)

target_link_libraries(${TARGET_NAME} PRIVATE minigin_lib)

# ---------------------------
# Windows DLL copy
# ---------------------------
if(WIN32)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

  foreach(dllTarget SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${dllTarget}>"
        "$<TARGET_FILE_DIR:${TARGET_NAME}>"
    )
  endforeach()

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/Data"
      "$<TARGET_FILE_DIR:${TARGET_NAME}>/Data"
  )
endif()