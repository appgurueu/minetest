local function clients_list_formspec(dialogdata)
	if not dialogdata or dialogdata.server == nil then
		return "size[10,11]" ..
			"button[3.5,10.25;3,0.8;dlg_clients_list_exit;OK]" ..
			"label[4,0.5;Error: Server data is missing]"
	end
	local clients_list = dialogdata.server.clients_list
	local servername   = dialogdata.server.name

	local function fmt_formspec_list(clients_list)
		local escaped = {}
		for i, str in ipairs(clients_list) do
			escaped[i] = core.formspec_escape(str)
		end
		return table.concat(escaped, ",")
	end

	local list = "textlist[1,1;7.75,9;client_list;" .. fmt_formspec_list(clients_list) .. "]"

	return
		"size[10,11]" ..
		"button[3.5,10.25;3,0.8;dlg_clients_list_exit;OK]" ..
		"label[4,0.5;This is the clientlist from " .. fgettext(servername) .. "]" ..
		list
end

--------------------------------------------------------------------------------

local function clients_list_buttonhandler(this, fields)
	if fields.dlg_clients_list_exit then
		this:delete()
		return true
	end
	return false
end

--------------------------------------------------------------------------------

function create_clientslist_dialog(server)
	local retval = dialog_create("dlg_clients_list",
		clients_list_formspec,
		clients_list_buttonhandler,
		nil)
	retval.data.server = server
	return retval
end
