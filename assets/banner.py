from PIL import Image, ImageDraw, ImageFilter, ImageFont
import math

W, H = 720, 320
cx, cy = W // 2, H // 2

# ── helpers ──────────────────────────────────────────────────────────────────
def deg2rad(d): return d * math.pi / 180

def wedge_pts(cx, cy, r, a1_deg, a2_deg, steps=12):
    pts = [(cx, cy)]
    for i in range(steps + 1):
        a = deg2rad(a1_deg + (a2_deg - a1_deg) * i / steps)
        pts.append((cx + r * math.sin(a), cy - r * math.cos(a)))
    return pts

def arc_fill(draw, cx, cy, r_out, r_in, a1_deg, a2_deg, color, steps=60):
    outer, inner = [], []
    for i in range(steps + 1):
        a = deg2rad(a1_deg + (a2_deg - a1_deg) * i / steps)
        outer.append((cx + r_out * math.sin(a), cy - r_out * math.cos(a)))
        inner.append((cx + r_in  * math.sin(a), cy - r_in  * math.cos(a)))
    poly = outer + list(reversed(inner))
    draw.polygon(poly, fill=color)

# ── glow layer factory ────────────────────────────────────────────────────────
def make_glow_layer(size, draw_fn, color, blur_r):
    layer = Image.new("RGBA", size, (0, 0, 0, 0))
    d = ImageDraw.Draw(layer)
    draw_fn(d, color)
    return layer.filter(ImageFilter.GaussianBlur(blur_r))

# ── colors ────────────────────────────────────────────────────────────────────
BG          = (0, 0, 0, 255)
MINT        = (0, 255, 170)       # lit minutes / steps
TEAL        = (0, 200, 255)       # lit hours  / battery
DIM         = (20, 60, 50)        # unlit minute tracks
OVERLAY_BG  = (0, 18, 14)
TEXT_WHITE  = (255, 255, 255)
TEXT_MINT   = (180, 255, 220)

# ── background starburst (blurred 24h face, slightly larger) ──────────────────
R_bg   = 310   # just bigger than half-height so it bleeds off edges
RING_T = 14
RING_G = 4
inset  = RING_T + RING_G
tick_r = R_bg - inset

bg_img = Image.new("RGBA", (W, H), (0, 0, 0, 255))
bg_d   = ImageDraw.Draw(bg_img)

# dim all ticks first
for g in range(12):
    a = 3 + 15 * g
    pts = wedge_pts(cx, cy, R_bg, a, a + 9)
    bg_d.polygon(pts, fill=(0, 40, 30))
    pts2 = wedge_pts(cx, cy, R_bg, 183 + 15*g, 183 + 15*g + 9)
    bg_d.polygon(pts2, fill=(0, 30, 50))

# lit minutes (right half: 0–30 min = groups 0–5)
for g in range(6):
    a = 3 + 15 * g
    pts = wedge_pts(cx, cy, R_bg, a, a + 9)
    bg_d.polygon(pts, fill=(0, 120, 90))

# lit hours (left half: 10 of 24 = 5 full slots + partial)
for h2 in range(5):
    a = 183 + 15 * h2
    pts1 = wedge_pts(cx, cy, R_bg, a, a + 3)
    pts2 = wedge_pts(cx, cy, R_bg, a + 6, a + 9)
    bg_d.polygon(pts1, fill=(0, 80, 120))
    bg_d.polygon(pts2, fill=(0, 80, 120))

bg_blur = bg_img.filter(ImageFilter.GaussianBlur(12))

# ── foreground face (sharp, right side, dark background to mask blur) ────────
FACE_CX = W - 150
FACE_CY = cy
R_fg    = 130
RING_FG = 8
RING_FG_G = 3
inset_fg = RING_FG + RING_FG_G

fg_img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
fg_d   = ImageDraw.Draw(fg_img)

# Dark background circle to mask the blurry starburst behind the face
fg_d.ellipse(
    [FACE_CX - R_fg - 2, FACE_CY - R_fg - 2,
     FACE_CX + R_fg + 2, FACE_CY + R_fg + 2],
    fill=(0, 0, 0, 255)
)

# dim tracks — individual minute ticks
for g in range(12):
    a_base = 3 + 15 * g
    for i in range(5):
        a = a_base + 2 * i
        pts = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 1)
        fg_d.polygon(pts, fill=DIM)
    pts2 = wedge_pts(FACE_CX, FACE_CY, R_fg, 183 + 15*g, 183 + 15*g + 9)
    fg_d.polygon(pts2, fill=(18, 45, 70))

# lit minutes (right, ~28 min = 5 full groups + 3 partial)
# Draw individual ticks (2deg each, 1deg gap) within each 5-min group (9deg block)
for g in range(5):
    a_base = 3 + 15 * g
    for i in range(5):
        a = a_base + 2 * i
        pts = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 1)
        fg_d.polygon(pts, fill=MINT)
# partial group: 3 ticks
a_base = 3 + 15 * 5
for i in range(3):
    a = a_base + 2 * i
    pts = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 1)
    fg_d.polygon(pts, fill=MINT)

# lit hours (left, 10h in 24h mode = 5 full slots)
for h2 in range(5):
    a = 183 + 15 * h2
    pts1 = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 3)
    pts2 = wedge_pts(FACE_CX, FACE_CY, R_fg, a + 6, a + 9)
    fg_d.polygon(pts1, fill=TEAL)
    fg_d.polygon(pts2, fill=TEAL)

# outer ring: battery right (80%), steps left (65%)
batt_end = 177 - int(174 * 0.80)
arc_fill(fg_d, FACE_CX, FACE_CY, R_fg, R_fg - RING_FG,  3,   177, (15, 40, 60))
arc_fill(fg_d, FACE_CX, FACE_CY, R_fg, R_fg - RING_FG, 183,  357, (10, 50, 35))
arc_fill(fg_d, FACE_CX, FACE_CY, R_fg, R_fg - RING_FG, batt_end, 177, TEAL)
arc_fill(fg_d, FACE_CX, FACE_CY, R_fg, R_fg - RING_FG, 183, 183 + int(174 * 0.65), MINT)

# overlay circle — empty (art mode, no text)
ov_r = 50
fg_d.ellipse(
    [FACE_CX - ov_r, FACE_CY - ov_r, FACE_CX + ov_r, FACE_CY + ov_r],
    fill=OVERLAY_BG
)

# ── glow passes on fg elements ────────────────────────────────────────────────
def draw_mint_ticks(d, col):
    for g in range(5):
        a_base = 3 + 15 * g
        for i in range(5):
            a = a_base + 2 * i
            pts = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 1)
            d.polygon(pts, fill=col)
    a_base = 3 + 15 * 5
    for i in range(3):
        a = a_base + 2 * i
        pts = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 1)
        d.polygon(pts, fill=col)

def draw_teal_ticks(d, col):
    for h2 in range(5):
        a = 183 + 15 * h2
        pts1 = wedge_pts(FACE_CX, FACE_CY, R_fg, a, a + 3)
        pts2 = wedge_pts(FACE_CX, FACE_CY, R_fg, a + 6, a + 9)
        d.polygon(pts1, fill=col)
        d.polygon(pts2, fill=col)

def draw_mint_ring(d, col):
    arc_fill(d, FACE_CX, FACE_CY, R_fg, R_fg - RING_FG, 183, 183 + int(174 * 0.65), col)

def draw_teal_ring(d, col):
    arc_fill(d, FACE_CX, FACE_CY, R_fg, R_fg - RING_FG, batt_end, 177, col)

glow_mint  = make_glow_layer((W, H), draw_mint_ticks, (0, 255, 170, 210), 12)
glow_teal  = make_glow_layer((W, H), draw_teal_ticks, (0, 200, 255, 210), 12)
glow_mring = make_glow_layer((W, H), draw_mint_ring,  (0, 255, 170, 190), 10)
glow_tring = make_glow_layer((W, H), draw_teal_ring,  (0, 200, 255, 190), 10)

# ── composite ─────────────────────────────────────────────────────────────────
out = Image.new("RGBA", (W, H), BG)
out.alpha_composite(bg_blur)
out.alpha_composite(fg_img)
out.alpha_composite(glow_mint)
out.alpha_composite(glow_teal)
out.alpha_composite(glow_mring)
out.alpha_composite(glow_tring)

# ── title text ────────────────────────────────────────────────────────────────
try:
    font_title  = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 78)
    font_two    = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 44)
    font_tag    = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 19)
except:
    font_title = font_two = font_tag = ImageFont.load_default()

td = ImageDraw.Draw(out)

# Text block in the right portion — face is at cx-130, radius 130, so right edge ~cx
TEXT_X = 32
TEXT_Y = cy - 44

# Measure "Radium" to place "2" right after with no overlap
r_bb = td.textbbox((TEXT_X, TEXT_Y), "Radium", font=font_title)
radium_right  = r_bb[2]
radium_bottom = r_bb[3]

two_x = radium_right + 8
two_y = TEXT_Y - 4

# Check it fits — nudge left if needed
two_bb = td.textbbox((two_x, two_y), "2", font=font_two)
if two_bb[2] > W - 12:
    shift = two_bb[2] - (W - 12)
    TEXT_X -= shift
    r_bb = td.textbbox((TEXT_X, TEXT_Y), "Radium", font=font_title)
    radium_right  = r_bb[2]
    radium_bottom = r_bb[3]
    two_x = radium_right + 8

# Glow on "Radium" — wide halo + tight core in mint
RADIUM_COLOR = (210, 248, 232)  # slightly mint-tinted white
glow_r1 = make_glow_layer((W, H),
    lambda d, c: d.text((TEXT_X, TEXT_Y), "Radium", font=font_title, fill=c),
    (*MINT, 140), 18)
out.alpha_composite(glow_r1)
glow_r2 = make_glow_layer((W, H),
    lambda d, c: d.text((TEXT_X, TEXT_Y), "Radium", font=font_title, fill=c),
    (*MINT, 80), 8)
out.alpha_composite(glow_r2)

# Extra glow on "Ra" only — atomic symbol nod, a touch brighter
glow_ra1 = make_glow_layer((W, H),
    lambda d, c: d.text((TEXT_X, TEXT_Y), "Ra", font=font_title, fill=c),
    (*MINT, 190), 22)
out.alpha_composite(glow_ra1)
glow_ra2 = make_glow_layer((W, H),
    lambda d, c: d.text((TEXT_X, TEXT_Y), "Ra", font=font_title, fill=c),
    (255, 255, 255, 130), 6)
out.alpha_composite(glow_ra2)

# Draw "Radium" mint-tinted white, "Ra" just a touch brighter/whiter
td.text((TEXT_X, TEXT_Y), "Radium", font=font_title, fill=RADIUM_COLOR)
td.text((TEXT_X, TEXT_Y), "Ra", font=font_title, fill=(245, 255, 250))

# Draw "2" mint
td.text((two_x, two_y), "2", font=font_two, fill=(*MINT, 255))

# Glow on "2"
glow_2 = make_glow_layer((W, H),
    lambda d, c: d.text((two_x, two_y), "2", font=font_two, fill=c),
    (*MINT, 255), 16)
out.alpha_composite(glow_2)
# second pass for extra intensity
glow_2b = make_glow_layer((W, H),
    lambda d, c: d.text((two_x, two_y), "2", font=font_two, fill=c),
    (*MINT, 180), 8)
out.alpha_composite(glow_2b)

# Tagline
tagline = "A Radial Bar Graph Watchface for All Pebbles"
tag_bb  = td.textbbox((0, 0), tagline, font=font_tag)
tag_x   = TEXT_X
tag_y   = radium_bottom + 52
td.text((tag_x, tag_y), tagline, font=font_tag, fill=(*TEXT_MINT, 240))

# ── save ──────────────────────────────────────────────────────────────────────
out_rgb = out.convert("RGB")
out_rgb.save("/mnt/user-data/outputs/radium2_banner.png", "PNG")
print("done")
