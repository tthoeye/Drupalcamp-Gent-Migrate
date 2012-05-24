<?php

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
  }

}