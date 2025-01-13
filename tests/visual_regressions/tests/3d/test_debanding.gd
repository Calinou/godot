extends Node


func _ready() -> void:
	get_viewport().use_debanding = true


func _exit_tree() -> void:
	get_viewport().use_debanding = false
