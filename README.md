# Route Planner

**This is a project being developed in the context of DA2425 - tp1 assignment**

## Objectives

The objective is to develop a path-planning tool supported by greedy algorithms that also emphasizes environmentally sustainable mobility alternatives.
It should identify the **fastest and second fastest** independent routes.
It should be able to plan routes that **exclude specific nodes or segments** (restricted routing).
It should be able to come up with environmentally friendly routes that **combine driving and walking with parking options**.
It should be able to deal with all sort of constraints, such as the **unavailability of certain routes**, or specific **mobility needs**.

## Notes
All streets are assumed to be two-way.

The urban map can be structured and represented as **weighted undirected graph**. Edges are the **street segments** that connect nodes which are **street intersections**.

Edges have two values associated - one represents the walking time to traverse it, the other represents the driving time.

Nodes have a boolean value associated which represents the existence of a parking lot in that place. It can be assumed that parking lots always have free room.

Every drivable segment is also walkable. However, there are segments where you can only walk.
