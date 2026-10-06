import sys
import pygame
import math
import time

def distance(x1, y1, x2, y2):
    return math.sqrt(math.pow((x1 - x2), 2) + math.pow((y1 - y2), 2))

def force(angle, mag, obj, dt):
    angle = math.radians(angle)
    obj["velX"] += ((math.cos(angle) * mag) / obj["mass"]) * dt
    obj["velY"] += ((math.sin(angle) * mag) / obj["mass"]) * dt

def impulse(angle, mag, obj):
    angle = math.radians(angle)
    obj["velX"] += ((math.cos(angle) * mag) / obj["mass"])
    obj["velY"] += ((math.sin(angle) * mag) / obj["mass"])

pygame.init()

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("My Pygame Window")

clock = pygame.time.Clock()
FPS = 60
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

obj = {
    "posX": 150,
    "posY": 400,
    "radius": 20,
    "velX":0,
    "velY": 0,
    "mass": 1
}


points = []

lines = []
for x in range(200):
    lines.append((x, 300 + x))
points.append(lines)

lines = []
for x in range(400):
    lines.append((200 + x, 500))
points.append(lines)

lines = []
for x in range(150):
    lines.append((600, 350 + x))
points.append(lines)

lines = []
for x in range(400):
    lines.append((200 + x, 350))
points.append(lines)

running = True

impulse(-45, 1100, obj)

gravity = 450 * obj["mass"]
friction = 0.05
restitution = 0.8

while running:
    dt = clock.tick(FPS) / 1000.0
    screen.fill((0, 0, 0))
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    force(270, gravity, obj, dt)
    

    closest = obj["radius"] + 2
    point = None
    for p in range(len(points)):
        point_found = False
        for q in range(len(points[p]) - 1):
            d = distance(points[p][q][0], points[p][q][1], obj["posX"], obj["posY"])
            if (d < obj["radius"]):
                if d < closest:
                    closest = d
                    point_found = True
                    point = points[p][q]

        if point_found:
            start = points[p][0]
            end = points[p][len(points[p]) - 1]
            try:
                new_angle = math.degrees(math.atan2(-end[1] - -start[1], end[0] - start[0])) % 360
            except:
                new_angle = 90

            side = (start[0] - end[0]) * (-obj["posY"] - -end[1]) - (-start[1] - -end[1]) * (obj["posX"] - end[0])

            if side < 0:
                new_angle += 90
            elif side > 0:
                new_angle -= 90

            new_angle = new_angle % 360

            current_vel = math.sqrt(math.pow(obj["velX"], 2) + math.pow(obj["velY"], 2))

            pt_dist = obj["radius"] - closest
            obj["posX"] += math.cos(math.radians(new_angle)) * pt_dist
            obj["posY"] -= math.sin(math.radians(new_angle)) * pt_dist

            obj["velX"] = obj["velX"] * (1 - friction)
            obj["velY"] = obj["velY"] * (1 - friction)

            normal_vect_x = math.cos(math.radians(new_angle)) * 1
            normal_vect_y = math.sin(math.radians(new_angle)) * 1

            speed_along_normal = obj["velX"] * normal_vect_x + obj["velY"] * normal_vect_y
            speed_along_tang = current_vel - speed_along_normal

            force(new_angle, -gravity, obj, dt)

            new_force = (-2 * speed_along_normal) * (restitution)

            impulse(new_angle, new_force * obj["mass"], obj)



    pygame.draw.circle(screen, WHITE, (obj["posX"], obj["posY"]), 20)
    for p in range(len(points)):
        for q in range(len(points[p]) - 1):
            pygame.draw.line(screen, WHITE, (points[p][q][0], points[p][q][1]), (points[p][q + 1][0], points[p][q + 1][1]))

    
    obj["posY"] -= obj["velY"] * dt
    obj["posX"] += obj["velX"] * dt
            

    pygame.display.flip()

pygame.quit()
sys.exit()
