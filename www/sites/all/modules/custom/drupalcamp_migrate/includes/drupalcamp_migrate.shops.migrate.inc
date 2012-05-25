<?php

module_load_include('inc', 'link', 'link.migrate');

class DrupalCampShopsMigration extends Migration {

  /**
   * Constructor
   *
   * This is where we define the Migrations mapping table,
   * The Source and the Destination, as well as general settings
   * such as a description and other optional stuff
   */
  public function __construct() {
  	parent::__construct();
    $this->description = t('Example migration of CSV shops into nodes.');

    // Define the Mapping Table
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'ID' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Content ID',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Define the Source
    $columns = array(
      0 => array('NAAM', 'Naam'),
      1 => array('CATEGORIE', 'Categorie'),
      2 => array('STRAAT', 'Straat'),
      3 => array('NUMMER', 'Nummer'),
      4 => array('POSTNUMMER', 'Postcode'),
      5 => array('STAD', 'Stad'),
      6 => array('AFBEELDING', 'Afbeelding'),
      7 => array('LINKS', 'Links'),
      8 => array('BESCHRIJVING', 'Beschrijving'),
      9 => array('ID', 'Content ID'),
    );

    $options = array(
      'delimiter' => ';',
      'header_rows' => 1
    );

    // Instantiate the source class using the path to the CSV file and the columns.
    $this->source = new MigrateSourceCSV(dirname(__FILE__) .'/gentverwent.csv.out', $columns, $options);
    // Instantiate the destination class
    $this->destination = new MigrateDestinationNode('shop');

    // Map title
    $this->addFieldMapping('title', 'NAAM')
         ->description('Mapping NAAM in Source to title of Node');
    // Map other simple text fields
    $this->addFieldMapping('field_address_street', 'STRAAT')
         ->description('Mapping NAAM in Source to title of Node');
    $this->addFieldMapping('field_address_number', 'NUMMER')
         ->description('Mapping NAAM in Source to title of Node');
    $this->addFieldMapping('field_address_postal_code', 'POSTNUMMER')
         ->description('Mapping NAAM in Source to title of Node');
    $this->addFieldMapping('field_address_municipality', 'STAD')
         ->description('Mapping NAAM in Source to title of Node');
    // Default values
    $this->addFieldMapping('language')
         ->defaultValue(LANGUAGE_NONE)
         ->description('Language');
    $this->addFieldMapping('comment')
         ->defaultValue(0)
         ->description('Comments disallowed by default');
    $this->addFieldMapping('status')
         ->defaultValue(1)
         ->description('Node published by default');
    // Map Taxonomy
    $this->addFieldMapping('field_category', 'CATEGORIE')
         ->description('Map and create taxonomy terms where necessary')
         ->arguments(array('create_term' => TRUE));
    // Map Body
    $this->addFieldMapping('body', 'BESCHRIJVING')
         ->arguments(array('format' => 'full_html'))
         ->description('Beschrijving');
    // Map a node reference
    $this->addFieldMapping('field_location', 'POSTNUMMER')
         ->sourceMigration('DrupalCampLocations')
         ->description('Node reference to locations');
    // Add an image
    $image_arguments = MigrateFileFieldHandler::arguments(NULL, 'file_copy', FILE_EXISTS_RENAME);
    $this->addFieldMapping('field_image', 'AFBEELDING')
         ->description('Copy and/or rename images');
    // Add multiple links
    $this->addFieldMapping('field_links', 'LINKS')
         ->separator(',')
         ->arguments(array('title' => 'This is a link'))
         ->description('Split and insert links');
    // Destination fields that are not going to be used
    $dnm = array(
      'is_new',
      'uid',
      'sticky',
      'created',
      'changed',
      'promote',
      'revision_uid',
      'revision',
      'path',
      'log',
      'tnid'
    );
    $this->addUnmigratedDestinations($dnm);

  }

  function prepareRow($row) {
    // Add lat and long to the row
    $coordinates = $this->getAddessCoordinates($row->STRAAT, $row->NUMMER, $row->POSTNUMMER, $row->STAD);
    $row->latitude = $coordinates['lat'];
    $row->longitude = $coordinates['lng'];
  }
  
  
  /**
   * Alter the fields after mapping
   */
  function prepare($node, stdClass $row) {
    $this->setTheCoordinates(&$node, $row);
  }


  /**
   * Retrieve the coordinates.
   *
   * @return
   *   an array with lat and lng coordinates and province, each empty if none found.
   */
  function getAddessCoordinates($street, $street_number, $postal_code, $city, $country='Belgium') {
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

}