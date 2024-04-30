ModCS.Npc.Act[361] = function(npc)

	-- Init
	if npc.act_no == 0 then
		if npc.direct ~= 0 then
			npc.act_no = 2
		end
		npc.act_no = 1

	-- Attack
	else
		-- Follow player
		if npc.x < ModCS.Player.x then
			npc.xm = npc.xm + 0.05
		else
			npc.xm = npc.xm - 0.05
		end
		if npc.y < ModCS.Player.y then
			npc.ym = npc.ym + 0.05
		else
			npc.ym = npc.ym - 0.05
		end
		-- Limit speed
		if npc.xm > 2.4 then
			npc.xm = 2.4
		elseif npc.xm < -2.4 then
			npc.xm = -2.4
		end
		if npc.ym > 2.4 then
			npc.ym = 2.4
		elseif npc.ym < -2.4 then
			npc.ym = -2.4
		end

		-- Bounce off walls
		if npc:TouchLeftWall() or npc:TouchRightWall() then
			npc.xm = -npc.xm / 0.5
		end

		-- Animate
		npc.ani_wait = npc.ani_wait + 1
		if npc.ani_wait > 3 then
			npc.ani_wait = 0
			npc.ani_no = npc.ani_no + 1
		end
		if npc.ani_no > 1 then
			npc.ani_no = 0
		end
	end

	npc:Move()

	if npc.x > ModCS.Player.x then
		npc:SetRect(96 + (npc.ani_no * 24), 176, 120 + (npc.ani_no * 24), 192)
	else
		npc:SetRect(96 + (npc.ani_no * 24), 160, 120 + (npc.ani_no * 24), 176)
	end
end

ModCS.Npc.Act[64] = function(npc)
    if (npc.act_no == 3 and npc:TouchFloor()) then
        ModCS.Npc.Spawn(4, npc.x, npc.y)
    end

    npc:ActCode()
end