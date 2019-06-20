function hack_numpad_press(keystr) --this function is called from hack dll when you press numpad from 0 to 9
	if keystr == "NUM0" then
		if level.present and db.actor ~= nil and db.actor:alive() then
			alife():create("af_medusa", db.actor:position(), db.actor:level_vertex_id(), db.actor:game_vertex_id())

			local pos = db.actor:position()
			local text = "Spawning medusa artefact: x="..pos.x.." y="..pos.y.." z="..pos.z

			db.actor:give_game_news(text, "ui\\ui_icons_task", Frect():set(300,100,50,50),0,0)
		end
	end
end