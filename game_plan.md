Capy Valley - GBA Life-Sim Game Plan

Concept
A GBA life-sim inspired by Stardew Valley. Scope is to reduced to fit hardware constraints (240x160 screen, 32KB IWRAM, 256KB EWRAM). Life-sim systems (relationships, events, time) are the priority; farming exists but is lighter.

All code must be beginner friendly: no abstrcations before they're needed, clear naming, short functions, inline comments or non-obvious GBA tricks only.

Core systems (priority order)

1. clock: day/night cycle, 4 seasons x 28 days, year counter
2. map/camera: tile-based (8x8 tiles, mode 0 bg), scrolling camera, 4-6 maps
3. player: move, interact, inventory (16-slot), stamina bar
4. npcs: 20-30 townfolk, daily schedules, heart system (0-10)
5. dialogue: simple text box engine, branching on heart level / season
6. farming: till, water, plant, harvest; season invalidates crops
7. animals: chicken (eggs), cows (milk); daily care raises quality
8. fishing: minigame (hold button to reel); fish vary by season/location
9. mining: dugeon floor (1 map), ore nodes, basic combat vs slimes
10. economy: sell produce at shop, buy seeds, tools, upgrades
11. events: town festivals (1/season), spouse events (heart-gated)
12. mariage: 3 male + 3 female spouses, gift, date, proposal flow
13. save: sram save (one slot), save on sleep

seasons & crops
each season = 28 in-game days. crops planted out-of-season wilt on day 1

spring: turnip, potato, strawberry, cauliflower
summer: tomato, blueberry, corn, melon, sunflower
fall: pumpkin, yam, eggplant, cranberry, amaranth
winter: no field crops, forage + greenhouse (late upgrade)

maps (6 total)

1. farm: player's plot, tilled land, coop, barn, house
2. town square: shop, blacksmith, clinic, notice board
3. forest: foraging, carpenter's house, witch's hut
4. beach: fishing pier, sailor's shack
5. mine entrance: leads to mine dungeon
6. mine: single scolling dungeon floor, ladders every 10 rooms

townfolks (26 npcs)
6 marriage candidates + 20 regular townfolk
marriage candidates:
male: finn (farmer), cole (miner), reef (sailor)
female: mira (artist), sable (witch), lena (doctor)
regular townsfolk: shopkeeper, blacksmith, mayor, priest, carpenter, kids, elders, etc. each has a name, daily routine, and 2-3 lines of seasonal dialouge

town events (4/year)
spring bloom: spring 24: flower gifting contest
summer fair: summer 14: fishing + cooking competition
harvest fest: fall 26: produce, judging, scarerow parade
winter market: winter 20: gift exchange, ice-skating cutsense

gba technical notes
render: mode 0 (4 tiled BGs). BG0=HUD, BG1=foreground, BG2=terrain, BG3=sky/backdrop
sprites: 8x8 or 16x16 obj tiles for player + npcs. palette 16-color per sprite group
no floats: fixed point q8 (1unit = 256 sub-pixels) for all positions
time: 1 real second = 1 in-game minute via VBlank counter (60fps -> 60 ticks/sec)
save: 32kb sram at 0x0E000000, write a small strcut with checksum
audio: sirect sound channel A/B for BGM, simple square-wave SFX via timer

development phases

phase 1 foundation: clock, map rendering, player movement, camerra scroll
phase 2 lifesim: npcs schedules, dialogue engine, heart system
phase 3 farming: tilling, watering, planting, harvesting, season wilt
phase 4 activities: fishing minigame, mining floor, animal care
phase 5 economy: shop, sell screen, tool, upgrades
phase 6 events: 4 town festivals, spouse events, marriage
phase 7 polish: audio, save/load, title screen, balance pass
