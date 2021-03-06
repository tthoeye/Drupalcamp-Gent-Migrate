<?php

class DrupalCampLocationsMigration extends Migration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Example migration of CSV locations into nodes.');
    global $language;
	$dl = $language->language;
    // for the key name and for source fields below.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'POSTNUMMER' => array(
          'type' => 'varchar',
          'length' => 32,
          'not null' => TRUE,
          'description' => 'Content ID',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Velden
    $fields = array(
      'ID' => t('Source ID'),
      'POSTNUMMER' => t('Postcode'),
      'STAD' => t('Stad'),
      'BESCHRIJVING' => t('Beschrijving')
    );

    $columns = array(
      4 => array('POSTNUMMER', 'Postcode'),
      5 => array('STAD', 'Stad'),
      8 => array('BESCHRIJVING', 'Beschrijving'),
    );

    $options = array(
       'delimiter' => ';',
       'header_rows' => 1
    );

    // Instantiate the source class using the path to the CSV file and the columns.
    $this->source = new MigrateSourceCSV(dirname(__FILE__) .'/gentverwent.csv.out', $columns, $options);
    // Instantiate the destination class
    $this->destination = new MigrateDestinationNode('location');


    // Define the field mappings
    $this->addFieldMapping('title', 'STAD')
         ->description('Stad wordt titel');
    $this->addFieldMapping('field_location_postal_code', 'POSTNUMMER')
         ->description('Postcode');
    $this->addFieldMapping('body', 'BESCHRIJVING')
    	 ->arguments(array('format' => 'full_html'))
         ->description('Beschrijving');
    $this->addFieldMapping('language')
         ->defaultValue($dl)
    	 ->description('Language');
    $this->addFieldMapping('comment')
         ->defaultValue(0)
    	 ->description('Comments disallowed by default');
    $this->addFieldMapping('status')
         ->defaultValue(1)
    	 ->description('Node published by default');	

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

}