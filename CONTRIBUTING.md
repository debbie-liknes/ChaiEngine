# General Style Guide

Many of these are taken from _API Design for C++_ by Martin Reddy.

## General Development

- Keep lines under 120 characters in length.
    - Extension for Visual Studio to
      assist: https://marketplace.visualstudio.com/items?itemName=PaulHarrington.EditorGuidelinesPreview
- Leverage SAST tooling:
    - https://marketplace.visualstudio.com/items?itemName=SonarSource.SonarLintforVisualStudio2022
- Write unit tests for library methods that have a well-defined interface.
    - GoogleTest is our library of choice for this.
    - Keep close to the source code. Consider using a `tests` subdirectory.

## C++ Guidelines

- Member variables should begin with the `m_` prefix.
- Leverage the PIMPL (pointer-to-implementation) idiom where possible.
    - Avoids unintentional API breakages.
    - Use `unique_ptr` types when using PIMPL interfaces to avoid forgetting to clean up memory.
    - Explicitly delete the copy constructors if you don't plan to define them.
  ```
  private:
      // Non-copyable (don't have a serialization process of the tree).
      dbvh(const dbvh&) = delete;
      const dbvh& operator=(const dbvh&) = delete;
  ```