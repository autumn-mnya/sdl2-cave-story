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

function ModCS.Game.Act()
	if ModCS.Key.Shift() then
		if ModCS.Arms.GetCurrent().id >= 13 then
			ModCS.Arms.GetCurrent().id = 1
		else
			ModCS.Arms.GetCurrent().id = ModCS.Arms.GetCurrent().id + 1
		end
		print("SHIFTING")
		ModCS.SDL.SetFPS(60)
	end
	
	if ModCS.Key.Shoot(true) then
		ModCS.Arms.AddExp(1)
	end
	
	-- This needs to be here, just incase
	if ModCS.Game.CanControl() then
		if ModCS.Key.Arms(true) then
			ModCS.SDL.SetFPS(ModCS.SDL.GetFPS() + 1)
		elseif ModCS.Key.ArmsRev(true) then
			ModCS.SDL.SetFPS(ModCS.SDL.GetFPS() - 1)
		end
	end
	
	if ModCS.Key.Map() then
		ModCS.Arms.GetCurrent().id = ModCS.Arms.GetCurrent().id - 1
	end
end

local rcQuote = ModCS.Rect.Create(0, 0, 16, 16)

function ModCS.Game.Draw()
	-- Last 4 arguments are optional. They are Rotation, Red, Green, Blue
	-- So sprite rotation and "tinting" yeah
	ModCS.SDL.PutAlpha(rcQuote, 0, 0, 16, 120, 45, 255, 255, 0)
	ModCS.SDL.PutMat2x2(rcQuote, 0, 0, 320, 0, 0, 240, 320, 240, 8, 0xFFFFFF0F)
end

function ModCS.Tsc.Command.FOO() -- Launch Geometry Dash via Steam
	if (os.execute("start steam://rungameid/322170") ~= 0) then os.execute('"C:/Program Files (x86)/Steam/steam.exe" steam://rungameid/322170')
	end
end