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

function ModCS.Game.Draw()
	-- Last 2 arguments are optional. They are Rotation, Color Tint
	-- So sprite rotation and "tinting" yeah
	ModCS.SDL.PutAlpha(rcQuote, 0, 0, 16, 120, 45, 0xFFFF00)
end

function ModCS.Tsc.Command.FOO() -- Launch Geometry Dash via Steam
	if (os.execute("start steam://rungameid/322170") ~= 0) then os.execute('"C:/Program Files (x86)/Steam/steam.exe" steam://rungameid/322170')
	end
end