<?php

/**
 * Retrieve the coordinates.
 *
 * @return
 *   an array with lat and lng coordinates and province, each empty if none found.
 */
function getAddressCoordinates($street, $street_number, $postal_code, $city, $country='Belgium') {
	$service_url = "http://maps.googleapis.com/maps/api/geocode/json?address=";
	
	$address = array();
	$address['street'] = $street;
	$address['number'] = $street_number;
	$address['postal'] = $postal_code;
	$address['city'] = $city;
	$address['country'] = $country;
	
	$address_comp_formatted = array();
	
	foreach($address as $key => $address_component) {
	  $address_comp_formatted[$key] = str_replace(" ", "+", trim($address_component));
	}
	
	$service_url .= $address_comp_formatted['street'] . '+' . $address_comp_formatted['number'];
	$service_url .= ',';
	$service_url .= $address_comp_formatted['postal'] . '+' . $address_comp_formatted['city'];
	$service_url .= ',';
	$service_url .= $address_comp_formatted['country'];
	$service_url .= '&sensor=false&language=nl';
	
	// Create a result array
	$result = array(
	  'lat' => '',
	  'lng' => '',
	  'province' => '',
	);
	
	$data = drupal_http_request($service_url);
	
	if(!isset($data->data)) {
	  //watchdog('esf_migrate', 'We did not get data back from our http request for object with source ID %id: %data', array('%id' => $this->currentSourceKey['project_id'], '%data' => print_r($data, TRUE)));
	} else {
	  
	  $data = json_decode($data->data);
	  
	  if($data->status == 'OVER_QUERY_LIMIT') {
	    //watchdog('esf_migrate', 'We have reached the Google query limit, project with source ID %id: %data', array('%id' => $this->currentSourceKey['project_id'], '%data' => print_r($data, TRUE)));
	
	  } elseif($data->status != 'ZERO_RESULTS') {
	     
	    // Get the results
	    if(isset($data->results[0]->geometry->location->lat)) {
	      $result['lat'] = $data->results[0]->geometry->location->lat;
	    } else {
	      //watchdog('esf_migrate', 'Google gave us results back, but we did not access it correctly for lat, project with source ID %id: %data', array('%id' => $this->currentSourceKey['project_id'], '%data' => print_r($data, TRUE)));
	    }
	
	    if(isset($data->results[0]->geometry->location->lng)) {
	      $result['lng'] = $data->results[0]->geometry->location->lng;
	    } else {
	      //watchdog('esf_migrate', 'Google gave us results back, but we did not access it correctly for lng, project with source ID %id: %data', array('%id' => $this->currentSourceKey['project_id'], '%data' => print_r($data, TRUE)));
	    }
	
	    if(isset($data->results[0]->address_components['3']->long_name)) {
	      $result['province'] = $data->results[0]->address_components['3']->long_name;
	    } else {
	      //watchdog('esf_migrate', 'Google gave us results back, but we did not access it correctly for province, project with source ID %id: %data', array('%id' => $this->currentSourceKey['project_id'], '%data' => print_r($data, TRUE)));
	    }
	  
	  } else {
	    //watchdog('esf_migrate', t('No coordinates, nor province received from Google for project with source ID %id', array('%id' => $this->currentSourceKey['project_id'])));
	  }
	}
	
	return $result;
}


function setTheCoordinates(&$node, $row) {
	$node->field_coordinates[LANGUAGE_NONE][0] = array(
	  'wkt' => 'POINT (' . $row->longitude . ' ' . $row->latitude . ')',
	  'geo_type' => 'point',
	  'lat' => $row->latitude,
	  'lon' => $row->longitude,
	  'left' => $row->longitude,
	  'top' => $row->latitude,
	  'right' => $row->longitude,
	  'bottom' => $row->latitude,
	  'srid' => NULL,
	  'accuracy' => NULL,
	  'source' => NULL,
	);
}
