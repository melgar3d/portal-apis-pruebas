<?php

/**
* @file
* Retrollamadas de página de administración para el módulo annotate.
*/

/**
* Generador de formularios. Configurar anotaciones.
*
* @ingroup forms
* @see system_settings_form().
*/

function annotate_admin_settings() {
	
	// Obtener una matriz de tipos de nodos con nomb res internos como claves y "nombre sencillos" como valores. 
	// Por ejemplo, array('page' => 'Basic Page', 'article' => 'Articles')
	
	$types = node_type_get_types();
	foreach($types as $node_type) {
		$options[$node_type->type] = $node_type->name;
	}
	
	$form['annotate_node_types'] = array(
		'#type' => 'checkboxes',
		'#title' => t('Users may annotate these content types'),
		'#options' => $options,
		'#default_value' => variable_get('annotate_node_types', array('page')),
		'#description' => t('A text field will be avaliable on these content types to make user-specific notes.'),
	);
	
	$form['annotate_deletion'] = array(
		'#type' => 'radios',
		'#title' => t('Annotations will be deleted'),
		'#description' => t('Select a method for deleting annotations.'),
		'#options' => array(
			t('Never'),
			t('Randomly'),
			t('After 30 days')
		),
		'#default_values' => variable_get('annotate_deletion', 0)  // Never de forma predeterminada
	);
	
	$form['annotate_limit_per_node'] = array(
		'#type' => 'textfield',
		'#title' => t('Annotations per node'),
		'#description' => t('Enter the maximum number of anotations allowed per node (0 for no limit).'),
		'#default_value' => variable_get('annotate_limit_per_node', 1),
		'#size' => 3
	);	
	
	$form['#submit'][] = 'annotate_admin_settings_submit';
	return system_settings_form($form, TRUE);
}

/**
* Procesar el envío de opciones de anotación.
*/

function annotate_admin_settings_submit($form, $form_state){
	// Iterar por las casillas de verificación de tipo de contenido mostrada en el formulario.
	foreach ($form_state['values']['annotate_node_types'] as $key => $value){
		// Si la casilla de verificación de un tipo de contenido no está marcada,
		// comprobar si este tipo de contenido tiene el campo annotation adjuntado por medio de la función field_info_instance.
		// En caso afirmativo, eliminamos el campo annotation cuando el administrador desactive la casilla.
		if(!$value){
			$instance = field_info_instance('node', 'annotation', $key);
			if (!empty($instance)){
				field_delete_instance($instance, false);
				watchdog("Annotation", 'Deleted annotation field from content type: %key', array('$key' => $key));
			}
		}else{
		// Si la casilla de verificación se marca para un tipo de contenido, comprobar si el campo está asociado a ese tipo de contenido.
		// En caso contrario, añadir el campo annotation al tipo de contenido.
		$instance = field_info_instance('node', 'annotation', $key);
		if (empty($instance)){
			$instance = array(
				'field_name' => 'annotation',
				'entity_type' => 'node',
				'bundle' => $key,
				'label' => t('Annotation'),
				'widget_type' => 'text_textarea_width_summary',
				'settings' => array('display_summary' => TRUE),
				'display' => array(
					'default' => array(
					'type' => 'text_default',
				),
				'teaser' => array(
					'type' => 'text_summary_or_trimmed',
				),
			),	
		); 
		$instance = field_create_instance($instance);
		watchdog('Annotation', 'Added annotation field to content type: %key', array('%key' => $key));
		}
		}
	}// Fin del bucle foreach
}