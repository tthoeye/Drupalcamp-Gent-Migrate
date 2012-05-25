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