<?php

/**
 * @file
 *
 * Transform the dataset here
 */


function drupalcamp_migrate_load_csv($file) { 

  $input = fopen(dirname(__FILE__) . '/' . $file, 'r');
  $output = fopen(dirname(__FILE__) . '/' . $file . '.out', 'w');

  $i = 0;
  while (($data = fgetcsv($input, 0, ';')) !== FALSE) {
  	$i++;
  	if ($i > 1) {
      $data[] = _random_image();
      $data[] = _random_links();
      $data[] = _random_html();
      $data[] = $i;
    }
    fputcsv($output, $data, ';');
    dsm($data);
  }
 
  fclose($input);
  fclose($output);
}

function _random_image() {
  $images = array(
    'http://www.inter.net/images/01.png',
    'http://www.inter.net/images/02.png',
    'http://www.inter.net/images/03.png',
    'http://www.inter.net/images/04.png',
    'http://www.inter.net/images/05.png',
  );
  return $images[rand(0, count($images) -1)];
}

function _random_links() {
  $links = array(
  	'http://www.puresign.be',
  	'http://www.calibrate.be',
  	'http://www.krimson.be',
  	'http://www.xio.be',
  	'http://www.openminds.be',
  	'http://www.nivario.be',
  	'http://www.amplexor.be',
  );
  $results = array();
  $num = rand(1,4);
  for($i = 0; $i < $num; $i++) {
  	$results[] = $links[rand(0, count($links) - 1)];
  }
  return implode(',', $results);
}

function _random_html() {
	return "<p><b>At vero eos</b></p><p> et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio.</p><p><i> Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus,</i></p><p> omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat</p>";
}