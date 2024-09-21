require("arms")
require("npc")

ModCS.Mod.SetName("AutPI")
ModCS.Mod.SetAuthor("autumn")

ModCS.Mod.SetOpening(13, 100, 1000)
ModCS.Mod.SetStart(13, 94, 10, 8)

ModCS.AddEntity("MyNewEntity")
ModCS.AddCaret("MyNewCaret")

--function ModCS.Game.Act()
--	ModCS.Npc.Spawn(361, 10, 10)
--end

local rcQuote = ModCS.Rect.Create(0, 0, 16, 16)
local rcMask = rcQuote

function AnimateMask()
	local rcLeft = {
		ModCS.Rect.Create(0+200, 0, 16+200, 16),
		ModCS.Rect.Create(16+200, 0, 32+200, 16),
		ModCS.Rect.Create(0+200, 0, 16+200, 16),
		ModCS.Rect.Create(32+200, 0, 48+200, 16),
		ModCS.Rect.Create(0+200, 0, 16+200, 16),
		ModCS.Rect.Create(48+200, 0, 64+200, 16),
		ModCS.Rect.Create(64+200, 0, 80+200, 16),
		ModCS.Rect.Create(48+200, 0, 64+200, 16),
		ModCS.Rect.Create(80+200, 0, 96+200, 16),
		ModCS.Rect.Create(48+200, 0, 64+200, 16),
		ModCS.Rect.Create(96+200, 0, 112+200, 16),
		ModCS.Rect.Create(112+200, 0, 128+200, 16),
	}
	
	local rcRight = {
		ModCS.Rect.Create(0+200, 16, 16+200, 32),
		ModCS.Rect.Create(16+200, 16, 32+200, 32),
		ModCS.Rect.Create(0+200, 16, 16+200, 32),
		ModCS.Rect.Create(32+200, 16, 48+200, 32),
		ModCS.Rect.Create(0+200, 16, 16+200, 32),
		ModCS.Rect.Create(48+200, 16, 64+200, 32),
		ModCS.Rect.Create(64+200, 16, 80+200, 32),
		ModCS.Rect.Create(48+200, 16, 64+200, 32),
		ModCS.Rect.Create(80+200, 16, 96+200, 32),
		ModCS.Rect.Create(48+200, 16, 64+200, 32),
		ModCS.Rect.Create(96+200, 16, 112+200, 32),
		ModCS.Rect.Create(112+200, 16, 128+200, 32),
	}
	
	-- Return if the player is hidden
	if (ModCS.Player.cond & 2 ~= 0) then
		return
	end
	
	-- Assign the appropriate rect based on the player's direction
	if (ModCS.Player.direct == 0) then
		rcMask = rcLeft[ModCS.Player.ani_no + 1]
	else
		rcMask = rcRight[ModCS.Player.ani_no + 1]
	end
end

-- Function to convert HSV to RGB
function HSVtoRGB(h, s, v)
    local r, g, b

    local i = math.floor(h * 6)
    local f = h * 6 - i
    local p = v * (1 - s)
    local q = v * (1 - f * s)
    local t = v * (1 - (1 - f) * s)

    i = i % 6

    if i == 0 then r, g, b = v, t, p
    elseif i == 1 then r, g, b = q, v, p
    elseif i == 2 then r, g, b = p, v, t
    elseif i == 3 then r, g, b = p, q, v
    elseif i == 4 then r, g, b = t, p, v
    elseif i == 5 then r, g, b = v, p, q
    end

    return math.floor(r * 255), math.floor(g * 255), math.floor(b * 255)
end

-- Update and animate the color
function GetRainbowColor()
    local time = os.clock()  -- Get time in seconds
    local hue = (time % 1)  -- Cycle through hue every second
    local r, g, b = HSVtoRGB(hue, 1, 1)  -- Full saturation and value for vibrant colors
    return (r << 16) | (g << 8) | b  -- Combine RGB into a single color
end

function ModCS.Game.DrawAbovePlayer()
	AnimateMask()
	
	-- Get the rainbow color based on time
	local rainbowColor = GetRainbowColor()

	-- Draw player mask with the rainbow color
	ModCS.SDL.PutAlpha(rcMask, ModCS.Player.x - 8 - ModCS.Camera.GetXPos(), ModCS.Player.y - 8 - ModCS.Camera.GetYPos(), 16, 255, 0, rainbowColor)
end
