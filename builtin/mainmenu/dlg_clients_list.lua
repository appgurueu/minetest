local function clientslist_formspec(dialogdata)
   return confirmation_formspec(
	fgettext("Are you sure you want to delete \"$1\"?", dialogdata.content.name),
		'dlg_delete_content_confirm', fgettext("Delete"),
		'dlg_delete_content_cancel', fgettext("Cancel")
   )
end

--------------------------------------------------------------------------------
local function clientslist_buttonhandler(this, fields)
    this.data.clients_list = fields.clients_list
    this.data.error = nil

    if fields.dlg_clients_list_close or fields.key_enter then
        this:delete()
    end
end

--------------------------------------------------------------------------------

function create_clientslist_dialog(content)
    local retval = dialog_create("dlg_clientslist",
			clientslist_formspec,
			clientslist_buttonhandler,
			nil)

    return retval
end
