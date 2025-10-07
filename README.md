# **This is a lab project for CSE 4108 Lab**

<br>

## **Here's Our Introduction:**

<br>

![image alt](https://github.com/KraKEn-bit/CSE_Project_SP2/blob/main/Image/PresentationWindow%2010_8_2025%2012_13_54%20AM.png?raw=true)


# **What Libraries did we use for our simulation game**?

Ans: ONLY **RAYLIB**  : ( 

### **Here's the official Raylib website from where you can learn about raylib library, its headers,in built functions etc**

[RAYLIB OFFICIAL](https://www.raylib.com/)


## **Here's an image of our game:**

![image alt](https://github.com/KraKEn-bit/CSE_Project_SP2/blob/main/Image/Media%20Player%2010_8_2025%2012_51_13%20AM.png?raw=true)


# **OUR GAME PROPERTIES:**

## 🎮 Overview

The simulation visualizes the **infection rate** across countries, where the opacity of infection circles gradually increases from transparent to fully visible.  
The sidebar dynamically displays:
- Total population alive  
- Infected and dead population  
- Percentage of healthy population (rounded for clarity)

The game can be played in **multiplayer mode**:
- **Player 1:** Spreads the infection using *Infect Points*  
- **Player 2:** Develops the cure using *Cure Points*

---

## 🧬 DISPLAY()

To represent infection visually:
- Opacity increases from `1/205 → 2/205 → 3/205 ... → 205/205 = 1.0 (max opacity)`
- As opacity increases, the sidebar updates global statistics
- The healthy population percentage is calculated and displayed dynamically

---

## 🦠 INFECT AND CURE POINTS

### 🔴 INFECT_POINTS

We introduced a global variable called `INFECT_POINTS`.  
These are **earned by Player 1** by interacting with infection bubbles.

#### 🧩 Usage:
Infect Points are the **currency** for upgrading virus capabilities and spread rate through the **Upgrade Menu**.  

Upgradable parameters include:
- 🌫️ Air / Water / Touch / Animal Spread  
- 🌡️ Temperature Adaptation (Cold / Heat)  
- ☠️ Kill Potential  
- 💉 Cure Resistance  

Each upgrade has a cost (in Infect Points) that increases with each purchase.  
Players must spend their points strategically to make the virus more potent and efficient.

---

### 🧪 CURE_POINTS

`CURE_POINTS` are **earned by Player 2**, who must press random on-screen keys at the right time to collect them.  
Each successful input gives `CURE_POINT++`.

These points are spent to improve cure development and post-cure control.

#### 🧠 Cure Upgrades (via `upgradeManage()`):
Each upgrade starts at **5 points**, with cost increasing per level.

Upgradable parameters include:
- ⚗️ Effort → Speeds up cure research  
- 🧬 Combat Infection → Reduces spread rate after cure  
- 💊 Combat Death → Reduces deaths after cure  

Maximum upgrade level: **5**

---

## 🖱️ MOUSE HOVER INFO (by Nuhiat)

Hovering the mouse over a country shows real-time info:
- Country name  
- Population  
- Current infected count  
- Current death count  
- % of healthy people  

**Implementation details:**
- `DrawRectangle()` → draws an info box near the hovered country  
- `DrawText()` → displays data (country name, population, infected, dead)  
- Highlights the hovered country (`INIT = num`)  

If a country is clicked **before the game starts**:
- That country’s infection count is set to `1`  
- `gameActive = 1` (infection begins spreading)

---

## 🧱 CURE STRUCT

The **Cure Structure** stores cure-related variables and progress metrics, managing upgrades, cooldowns, and research speed as the cure develops over time.

---

## ⚙️ UPGRADE_MANAGE()

This function controls the entire upgrade logic for both infection and cure.

### Variables:
- `current_key`  
- `next_key_time`  
- `generate_next_time`  
- `randCountry`  

#### Player 1 (Virus Upgrades)
- Spends **Infect Points** on virus traits.  
- Upgrades increase infection rate and survivability.  
- Costs scale over time — the player must plan strategically.

#### Player 2 (Cure Upgrades)
- Spends **Cure Points** on cure traits.  
- Keys `[7]`, `[8]`, and `[9]` trigger respective cure upgrades.  

Each upgrade only activates if:
- Player has enough points  
- Upgrade level < 5  

---

## 💀 INFECTION BUBBLES (Purple Skulls)

These are *collectable* elements for Player 1.

### Spawn Conditions:
A country gets added to the activation queue when:
- First infection occurs  
- Infection reaches a certain time threshold  
- Infection surpasses **70%** of population  

Each queued country triggers a delayed bubble spawn.  
Cooldown prevents more than one bubble from spawning too soon (`15 × FPS` delay).

### Player Interaction:
- Click on the bubble → `+1 Infection Point`  
- Bubble disappears after collection  
- Short cooldown before the next bubble appears  

---

## 💚 CURE BUBBLES (Green Circles)

Exclusive to **Player 2 (Cure)**.

### Appearance:
- Appear randomly in technologically advanced countries  
- Display a random number (e.g., `1`, `2`, `3`)  

### Interaction:
- When the player presses the corresponding number key → `+1 Cure Point`

---

## 🔁 POINT SYSTEM FUNCTIONS

### `points_update()`
- Checks each country’s infection status  
- Determines when a bubble should appear  
- Adds countries to the **activation queue** once conditions are met  

### `display_bubble()`
- Draws and animates active bubbles  
- Handles player click detection and reward addition  
- Applies cooldowns to prevent over-spawning  

### `trackBubble()`
- Tracks all active bubbles  
- Manages countdowns and removal of expired bubbles  

---

## 🧩 SUMMARY OF GAMEPLAY LOOP

1. Hover over countries to see stats  
2. Click to start infection in a chosen country  
3. Player 1 collects Infection Points via bubbles  
4. Player 2 earns Cure Points via keyboard challenges  
5. Both use their points to upgrade their side:
   - Virus upgrades (spread, adaptation, lethality)  
   - Cure upgrades (research, infection resistance, death reduction)  
6. The world’s fate depends on how efficiently each player acts

---

## 🧠 Future Plans

- Add sound effects for infection/cure bubbles  
- Introduce AI mode (single-player option)  
- Implement global events (lockdowns, mutations, vaccine failures)  
- Add leaderboard for competitive play  

---

