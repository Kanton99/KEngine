```mermaid
---
config:
  theme: mc
  look: classic
  layout: elk
---
classDiagram
    class System {
	    +Update() void
	    +Start() void
    }
    class Transform {
	    -transform: mat4
	    +position() vec3
    }
    class ComponentManager {
	    +components: Map[ComponentType, Component[]]
    }
    class SystemManager {
	    +systems: System[]
	    +startSystems()
	    +updateSystems()
        +addSystem(System system)
    }
    class Component {
	    +entity: Entity
    }
    class Entity {
	    +name: String
	    +id: uuid
    }
    class EntityManager {
	    -entities: Entity[]
	    +createEntity() Entity
        +destroyEntity(id)
	    +getEntity() Entity
    }

    Component <|-- Transform
    EntityManager "1" <-- "*" Entity
    ComponentManager "1" <-- "*" Component
    SystemManager "1" <-- "*" System
    Component <.. Entity
```
