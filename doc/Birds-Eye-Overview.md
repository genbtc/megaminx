Got it — let’s restate the *big-picture architecture* of your Megaminx project as if we were onboarding a new developer who 
 needs to understand how the whole thing fits together, from data representation → puzzle logic → solver → rendering.

## 🧩 Megaminx — Bird’s-Eye Logic Overview

### 1. Core Concept

The project is a logical and visual simulator of a Megaminx (a 12-sided Rubik’s-like dodecahedron puzzle).
Everything revolves around a single data model of the puzzle state — how its *pieces* are oriented and permuted — 
 and the operations (rotations, algorithms) that mutate that state.

### 2. The Engine Layer (`src/engine/`)

This is the heart of the project: it defines what a Megaminx *is* in code, and how its state can change.

#### a. `piece*.hpp` — The Building Blocks

These files define the *atomic elements* of the puzzle:

* Corners, edges, and centers each get their own struct.
* Each piece struct holds:

* A unique identifier (index or enum)
* Orientation and positional data
* Possibly adjacency or color/face mapping info (depending on how you visualize)
* Enums define all the possible positions/orientations a piece can take.

Essentially, this layer describes:

> “What kinds of physical parts exist, and how they can be distinguished.”

#### b. `face.hpp` — Grouping Pieces Into Faces

This layer defines the 12 faces of the Megaminx and their relationships.

* Each face knows:

* Which pieces belong to it (corner/edge references)
* How they are arranged (ordering, rotation direction)
* Adjacencies to neighboring faces

It bridges the *local piece geometry* and the *global topology* of the dodecahedron.

Think of `face.hpp` as:

> “If you twist this face, which pieces rotate, and where do they go?”

#### c. `algorithms.hpp` — Abstract Move Definitions

This file describes the moves and algorithms available:

* Each basic rotation (e.g. "U", "R", "L", "F", etc.) is mapped to a set of transformations on pieces/faces.
* It may define *compound* algorithms as sequences of basic moves (e.g. PLL or orientation routines).
* Provides symbolic names for those transformations, so user commands or solver scripts can refer to them textually.

This is the abstraction layer that translates *“algorithm strings”* → *actual state mutations*.

> “Given a move symbol or algorithm name, what does it mean in terms of piece movement?”

#### d. `megaminx.cpp` — Assembling the Puzzle

This is the constructor and coordinator of the entire puzzle. It:

* Instantiates all faces and their relationships
* Initializes piece positions/orientations
* Provides functions to apply moves (by referencing `algorithms.hpp`)
* Keeps a canonical `PuzzleState` (or equivalent) representing the current configuration
* Interfaces with the rendering/UI layer via transformation data

> “This is the living puzzle — it can be scrambled, rotated, drawn, or solved.”

#### e. `solve.cpp` — Transforming Logic Into Action

This is the control layer where *algorithms meet execution*.

* Accepts high-level algorithm input (from text, keyboard, or solver automation)
* Parses it into a sequence of atomic face rotations
* Calls into the Megaminx object to apply each move (which mutates the underlying data model)
* Optionally drives solver logic:

* Layer-by-layer routines
* Specific heuristics or pattern detection
* Produces a final sequence of rotations that can be visualized or validated

> “This is where algorithmic intelligence turns into actual puzzle moves.”

### 3. Rendering / UI Layer (`src/ui/` or top level)

Once the puzzle state exists, the UI takes that state and renders it.

* Uses OpenGL to draw the dodecahedron based on the positions/orientations of the pieces in the engine.
* Maps piece colors → face polygons → rotation matrices.
* May handle input (keyboard/mouse) to trigger moves or algorithms through the solver interface.

> “Render the current state, animate the rotations, and accept user commands.”

### 4. Data Flow Summary

```
algorithms.hpp      -- defines → symbolic moves
piece*.hpp, face.hpp -- define → physical topology
megaminx.cpp        -- constructs → puzzle state + applies moves
solve.cpp           -- interprets → algorithm sequences into move calls
UI / opengl.cpp     -- visualizes → current state from the engine
```

In short:

* `piece*.hpp` — what the puzzle *is made of*
* `face.hpp` — how pieces form *faces*
* `algorithms.hpp` — what *moves* exist
* `megaminx.cpp` — how it all *fits together* and moves
* `solve.cpp` — how to *execute* algorithms and *solve*
* `ui/opengl.cpp` — how to *see* and *interact* with it

---

Would you like me to go one layer deeper next — e.g. explaining how a move definition (from algorithms.hpp) actually transforms
  the piece data structures (through face rotation mapping)? That’s the core mechanism tying the symbolic and physical models together.
