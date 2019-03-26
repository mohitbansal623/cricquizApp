<?php

function checkLogin($username, $password) {

$check_uid_exists = db_select('users', 'u')
                    ->fields('u', array('uid'))
                    ->condition('u.name', $username);
$uid = $check_uid_exists->execute()->fetchField();

$password_check = 0;

if (!empty($uid)) {
    $user_info = user_load($uid);
    module_load_include('inc', 'content', 'includes/password');
    $password_check = user_check_password($password, $user_info);
}

else {
$error_message = array('error' => 'Invalid Login');
return $error_message;
}

$result = $check_uid_exists->execute()->rowCount();


if ($result == 1 && $password_check == 0) {
  return 'Invalid Login';
}

elseif ($result == 1 && $password_check == 1) {
  $send_uid = array('uid' => $uid, 'message' => 'Successfully Login');
  return $send_uid;
}


}


function checkLoginAccess($username, $password) {
return TRUE;
}

function checkCreateUserAccess($username, $email, $password) {
return TRUE;
}

function createUser($username, $email, $password, $number, $city, $state, $pin, $user_referal_code) {

$status = check_login_exists($username, $email);
if ($status == 'Already Exists' || $status == 'User Name Already Exists' || $status == 'Email Already Exists') {

   $message = array('error' => $status);
   return $message;
}

else {

$referal_code = strtoupper($username) . "123";

// CHECK User Referal Code Exists or Not
$query = db_select('users', 'u')
           ->fields('u', array('uid'))
           ->condition('r.field_referal_code_value', $user_referal_code);

$query->join('field_data_field_referal_code', 'r', "u.uid = r.entity_id");
$uid = $query->execute()->fetchField();

if (empty($uid)) {
$message = array('error' => 'Referal Code does not exists');
return $message;
}
else {
$new_user = array(
  'name' => $username,
  'pass' => $password, // note: do not md5 the password
  'mail' => $email,
  'status' => 1,
  'init' => $email,
  'roles' => array(
    DRUPAL_AUTHENTICATED_RID => 'authenticated user',
  ),

'field_mobile_number' => array(
    'und' => array(
      0 => array(
        'value' => $number,
      ),
    ),
  ),
  'field_city' => array(
    'und' => array(
      0 => array(
        'value' => $city,
      ),
    ),
  ),
  'field_state' => array(
    'und' => array(
      0 => array(
        'value' => $state,
      ),
    ),
  ),
  'field_pin_code' => array(
    'und' => array(
      0 => array(
        'value' => $pin,
      ),
    ),
  ),
  
  'field_referal_code' => array(
    'und' => array(
      0 => array(
        'value' => $referal_code,
      ),
    ),
  ),
  
  'field_add_referal_user_id' => array(
    'und' => array(
      0 => array(
        'target_id' => $uid,
      ),
    ),
  ),
  
  'field_cash_bonus' => array(
    'und' => array(
      0 => array(
        'value' => 100,
      ),
    ),
  ),
  
  'field_add_user_referal_code' => array(
    'und' => array(
      0 => array(
        'value' => $user_referal_code,
      ),
    ),
  ),
);

// The first parameter is sent blank so a new user is created.
user_save('', $new_user);

$message = array('error' => 'New User created');
return $message;


}
}


}


function check_login_exists($username, $email) {


$check_uid_exists = db_select('users', 'u')
                    ->fields('u', array('uid'))
                    ->condition('u.name', $username);

$result = $check_uid_exists->execute()->rowCount();

if ($result == 1) {
    $message = 'User Name Already Exists';
    return $message;
}

else {


$check_uid_exists = db_select('users', 'u')
                    ->fields('u', array('uid'))
                    ->condition('u.mail', $email);
                    
$result = $check_uid_exists->execute()->rowCount();

if ($result == 1) {
     $message = 'Email Already Exists';
return $message;
}

else {
   $message = 'Create User';
   return $message;
 }
}

}




function checkMatchAccess($username) {
return TRUE;
}


function checkQuizAccess($quizid) {
  return TRUE;

}

function getlistMatches($username) {
   
    //$current_time = REQUEST_TIME;
    $current_time = date('Y-m-d h:i:s',strtotime('+5 hour +30 minutes',REQUEST_TIME));
    $current = strtotime('+5 hour +30 minutes',REQUEST_TIME);

//$current_time = date('Y-m-d h:i:s');
$query = db_select('node', 'n')
           ->fields('n', array('nid'))
           ->condition('n.type', 'ipl_matches')
           ->condition('n.status', 1);
        $query->join('field_data_field_betting_time', 'bet', "n.nid = bet.entity_id");
      // $query->condition('bet.field_betting_time_value' , $current_time, '>');
           $query->orderby('n.created', 'DESC');
    $query->range(0,15);       
    $nids = $query->execute()->fetchCol();
$matches = array();
if (!empty($nids)) {
foreach($nids as $key => $value) {
$node = node_load($value);
$matches['matches'][$key]['location'] = $node->field_match_location['und'][0]['value'];
$matches['matches'][$key]['title'] = $node->title;
$matches['matches'][$key]['nid'] = $node->nid;

$matches['matches'][$key]['team_a'] = $node->field_team_a['und'][0]['value'];
$matches['matches'][$key]['team_b'] = $node->field_team_b['und'][0]['value'];

$matches['matches'][$key]['team_icon_b'] = file_create_url($node->field_team_b_icon['und'][0]['uri']);
$matches['matches'][$key]['team_icon_a'] = file_create_url($node->field_team_a_icon['und'][0]['uri']);
$match_date = date("Y-m-d" ,strtotime($node->field_match_date['und'][0]['value']));
$matches['matches'][$key]['date'] = $match_date;
$match_time =  strtotime($node->field_match_date['und'][0]['value']); 
$bet_time =  strtotime($node->field_betting_time['und'][0]['value']);
$diff = $bet_time - $current;
$remaining = date("h:i:s", $diff);
$matches['matches'][$key]['remaining_days'] = $remaining;

}
//$matches = json_encode($matches);

return $matches;
}
    else {
      $message = array('error' => 'No matches yet');
   return $message;

    }
}


function getlistQuestions($quizid) {

$query = db_select('node', 'n')
              ->fields('n', array('nid'))
              ->fields('a', array('field_option_a_value'))
              ->fields('b', array('field_option_b_value'))
              ->fields('c', array('field_option_c_value'))
              ->fields('d', array('field_option_d_value'))
              ->fields('q', array('field_question_value'))
             ->condition('n.type', 'questions')
             ->condition('n.status', 1);

$query->condition('id.field_quiz_id_target_id', $quizid);            
$query->join('field_data_field_option_a', 'a', "a.entity_id = n.nid");
$query->join('field_data_field_option_b', 'b', "b.entity_id = n.nid");
$query->join('field_data_field_option_c', 'c', "c.entity_id = n.nid");
$query->join('field_data_field_option_d', 'd', "d.entity_id = n.nid");
$query->join('field_data_field_question', 'q', "q.entity_id = n.nid");
$query->join('field_data_field_quiz_id', 'id', "id.entity_id = n.nid");
$query->range(0,10);

$questions = $query->execute()->fetchAll();
$ques = array();

foreach ($questions as $key => $value) {
$ques['questions'][$key]['ques_id'] = $value->nid;
$ques['questions'][$key]['ques'] = $value->field_question_value;
$ques['questions'][$key]['option_a'] = $value->field_option_a_value;
$ques['questions'][$key]['option_b'] = $value->field_option_b_value;
$ques['questions'][$key]['option_c'] = $value->field_option_c_value;
$ques['questions'][$key]['option_d'] = $value->field_option_d_value;
}

//$ques = json_encode($ques);

  
return $ques;
}


function getlistQuiz($quizid) {

    $current_time = date('Y-m-d h:i:s',strtotime('+5 hour +30 minutes',REQUEST_TIME));
$current = strtotime('+5 hour +30 minutes',REQUEST_TIME);
$query = db_select('node', 'n')
              ->fields('n', array('nid', 'title'))
              ->fields('qiz', array('field_quiz_time_value'))
              ->fields('bet', array('field_quiz_participate_end_time_value'))
             ->condition('n.type', 'quiz')
             ->condition('n.status', 1);
             $query->orderby('n.created', 'DESC');
        $query->join('field_data_field_quiz_participate_end_time', 'bet', "n.nid = bet.entity_id");
        $query->join('field_data_field_quiz_time', 'qiz', "n.nid = qiz.entity_id");
      //  $query->condition('bet.field_quiz_participate_end_time_value' , $current_time, '>');
      $query->range(0,15);       

 $all_quizes  = $query->execute()->fetchAll(); 
foreach ($all_quizes as $key => $value) {
$quiz['quizes'][$key]['quizid'] = $value->nid;
  $quiz['quizes'][$key]['title'] = $value->title;
  $quiz['quizes'][$key]['start_time'] = date('h:i a', strtotime($value->field_quiz_time_value));
$quiz['quizes'][$key]['end_time'] = date('h:i a', strtotime($value->field_quiz_participate_end_time_value));
$end = strtotime($value->field_quiz_participate_end_time_value);
$diff = $end - $current;
$remaining = date("h:i:s", $diff);
$quiz['quizes'][$key]['remaining']  = $remaining;
}


return $quiz;
}

function checkCricketFormAccess($matchid) {
 return TRUE;
}


function getCricketFormData($matchid) {
  if (is_numeric($matchid)) {
  	$node = node_load($matchid);
  	$options = array();
  	$options['team_a_name'] = $node->field_team_a['und'][0]['value'];
  	$options['team_b_name'] = $node->field_team_b['und'][0]['value'];
  	
  	$team_aname = $node->field_team_a['und'][0]['value'];
  	$team_bname = $node->field_team_b['und'][0]['value'];
    
    $alpha = array('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j');
    
  	$top_batsman_team_a = $node->field_top_batsman_of_team_a['und'];
	if (!empty($top_batsman_team_a)) {
		foreach($top_batsman_team_a as $key => $value)  {
		$options['top_batsman_team_a'][] = $value['value'];
		
		// $top_batsman_a["option_" . $alpha[$key]]= $value['value'];
		}
	}
	
	print_r($top_batsman_a);
	print_r("\n");

	$top_batsman_team_b = $node->field_top_batsman_of_team_b['und'];

	if (!empty($top_batsman_team_b)) {
		foreach($top_batsman_team_b as $key => $value)  {
		$options['top_batsman_team_b'][] = $value['value'];
		$top_batsman_b .= ' option_' . $alpha[$key] . '=> ' . $value['value'] . ',';
		}
	}
	

	$top_bowler_team_a = $node->field_top_bowler_of_team_a['und'];

	if (!empty($top_bowler_team_a)) {
		foreach($top_bowler_team_a as $key => $value)  {
		$options['top_bowler_team_a'][] = $value['value'];
		
		$top_bowler_a .= ' option_' . $alpha[$key] . '=> ' . $value['value'] . ',';
		}
	}

	$top_bowler_team_b = $node->field_top_bowler_of_team_b['und'];

	if (!empty($top_bowler_team_b)) {
		foreach($top_bowler_team_b as $key => $value)  {
		$options['top_bowler_team_b'][] = $value['value'];
		
	    $top_bowler_b .= ' option_' . $alpha[$key] . ' => ' . $value['value'];
		}
	}
	
	$man_of_the_match = $node->field_man_of_the_match['und'];

	if (!empty($man_of_the_match)) {
		foreach($man_of_the_match as $key => $value)  {
		$options['man_of_the_match'][] = $value['value'];
				}
	}
  }
  
  $options = array(
     
     array('quesid' => 1, 'question' => 'MATCH TOSS', 'options' => array($team_aname, $team_bname)), 
  
    array('quesid' => 2, 'question' => 'HIGHEST POWERPLAY RUNS', 'options' => array($team_aname, $team_bname,'Tie')),
    
    array('quesid' => 3, 'question' => 'HIGHEST WICKET FALL', 'options' => array($team_aname,$team_bname,'Tie')),
    
    array('quesid' => 4, 'question' => 'HIGHEST 4S IN MATCH', 'options' => array($team_aname, $team_bname,'Tie')),
    
    array('quesid' => 5, 'question' => 'HIGHEST 6S IN MATCH', 'options' => array($team_aname, $team_bname, 'Tie')),
    
    array('quesid' => 6, 'question' => 'EXTRA RUNS GIVEN', 'options' => array($team_aname, $team_bname,'Tie')),
    
    array('quesid' => 7, 'question' => 'TOP RUN SCORER', 'options' => array($team_aname, $team_bname, 'Tie')),
    
    array('quesid' => 8, 'question' => 'TOP BOWLER IN MATCH', 'options' => array($team_aname, $team_bname)),
    
    array('quesid' => 9, 'question' => 'HIGHEST OPENING PARTNERSHIP', 'options' => array($team_aname, $team_bname, 'Tie')),
    
    array('quesid' => 10, 'question' => 'HIGHEST CATCH OUT', 'options' => array($team_aname, $team_bname, 'Tie')),
    
    array('quesid' => 11, 'question' => 'HIGHEST DISMISSAL BY KEEPER', 'options' => array($team_aname,$team_bname, 'Tie')),
    
    array('quesid' => 12, 'question' => 'PLAYER SCORE 50', 'options' => array($team_aname,  $team_bname,  'Tie')),
    
    array('quesid' => 13, 'question' => '50+ SCORE IN MATCH', 'options' => array(0,  1 ,  2,'3+' )),
    
    array('quesid' => 14, 'question' => 'HIGHEST POWERPLAY BOUNDARIES', 'options' => array($team_aname, $team_bname, 'Tie')),
    
    array('quesid' => 15, 'question' => 'TOP BATSMAN TEAM A', 'options' => array( $options['top_batsman_team_a'][0] ,  $options['top_batsman_team_a'][1], $options['top_batsman_team_a'][2],  $options['top_batsman_team_a'][3], $options['top_batsman_team_a'][4],  $options['top_batsman_team_a'][5],  $options['top_batsman_team_a'][6], $options['top_batsman_team_a'][7], $options['top_batsman_team_a'][8],  $options['top_batsman_team_a'][9] )),
    
    
    
    array('quesid' => 16, 'question' => 'TOP BATSMAN TEAM B', 'options' => array($options['top_batsman_team_b'][0] ,  $options['top_batsman_team_b'][1], $options['top_batsman_team_b'][2],  $options['top_batsman_team_b'][3],  $options['top_batsman_team_b'][4],  $options['top_batsman_team_b'][5],  $options['top_batsman_team_b'][6],  $options['top_batsman_team_b'][7], $options['top_batsman_team_b'][8],  $options['top_batsman_team_b'][9] )),
    
    
    array('quesid' => 17, 'question' => 'TOP BOWLER TEAM A', 'options' => array( $options['top_bowler_team_a'][0] ,  $options['top_bowler_team_a'][1], $options['top_bowler_team_a'][2],  $options['top_bowler_team_a'][3],  $options['top_bowler_team_a'][4],  $options['top_bowler_team_a'][5],  $options['top_bowler_team_a'][6], $options['top_bowler_team_a'][7],  $options['top_bowler_team_a'][8], $options['top_bowler_team_a'][9] )),
    
    array('quesid' => 18, 'question' => 'TOP BOWLER TEAM B', 'options' => array($options['top_bowler_team_b'][0] ,  $options['top_bowler_team_b'][1], $options['top_bowler_team_b'][2],  $options['top_bowler_team_b'][3],  $options['top_bowler_team_b'][4], $options['top_bowler_team_b'][5],  $options['top_bowler_team_b'][6],  $options['top_bowler_team_b'][7], $options['top_bowler_team_b'][8],  $options['top_bowler_team_b'][9] )),
    
    array('quesid' => 19, 'question' => 'MAN OF THE MATCH', 'options' => array($options['man_of_the_match'][0] ,  $options['man_of_the_match'][1], $options['man_of_the_match'][2],  $options['man_of_the_match'][3],  $options['man_of_the_match'][4],  $options['man_of_the_match'][5],  $options['man_of_the_match'][6],  $options['man_of_the_match'][7], $options['man_of_the_match'][8],  $options['man_of_the_match'][9] )),
    
    
    
    array('quesid' => 20, 'question' => 'MATCH WINNER', 'options' => array($team_aname, $team_bname,'Tie')),
  
  );
  
  //$options = (object) $options;
  
  $object = new stdClass();
  $object->data = $options;
  
  return $object;
}

function getListofContests($matchid) {
$query = db_select('node', 'n')
     ->fields('n', array('nid', 'title'))
     //->fields('pool', array('field_prize_pool_value'))
     ->fields('fee', array('field_entry_fee_value'))
     ->condition('n.status', 1)
     ->condition('n.type', 'match_contests');
$query->condition('mat.field_match_id_target_id', $matchid);
//$query->join('field_data_field_prize_pool', 'pool', "pool.entity_id = n.nid");
$query->join('field_data_field_entry_fee', 'fee', "fee.entity_id = n.nid");
$query->join('field_data_field_match_id', 'mat', "mat.entity_id = n.nid");

$results = $query->execute()->fetchAll();

$contests = array();
if (!empty($results)) {
foreach($results as $key => $value) {
$contests['contests'][$key]['nid'] = $value->nid;
$contests['contests'][$key]['title'] = $value->title;
//$contests['contests'][$key]['prize_value'] = "Rs " . $value->field_prize_pool_value;
$contests['contests'][$key]['entry_fee'] = "Rs " . $value->field_entry_fee_value;
}
}

return $contests;
}



function getListQuizContests($quizid) {
$query = db_select('node', 'n')
     ->fields('n', array('nid', 'title'))
     ->fields('pool', array('field_prize_pool_value'))
     ->fields('fee', array('field_entry_fee_value'))
     ->fields('spots', array('field_total_spots_value'))
     ->fields('win', array('field_total_winners_value'))
     ->condition('n.status', 1)
     ->condition('n.type', 'quiz_contests');
$query->condition('mat.field_add_quiz_id_target_id', $quizid);
$query->join('field_data_field_prize_pool', 'pool', "pool.entity_id = n.nid");
$query->join('field_data_field_entry_fee', 'fee', "fee.entity_id = n.nid");
$query->join('field_data_field_add_quiz_id', 'mat', "mat.entity_id = n.nid");
$query->join('field_data_field_total_spots', 'spots', "spots.entity_id = n.nid");
$query->join('field_data_field_total_winners', 'win', "win.entity_id = n.nid");
$results = $query->execute()->fetchAll();
$contests = array();
if (!empty($results)) {
foreach($results as $key => $value) {
$contests['contests'][$key]['nid'] = $value->nid;
$nid = $value->nid;
$contests['contests'][$key]['title'] = $value->title;
$contests['contests'][$key]['prize_value'] = "Rs " . $value->field_prize_pool_value;
$contests['contests'][$key]['entry_fee'] = "Rs " . $value->field_entry_fee_value;
$contests['contests'][$key]['spots'] = $value->field_total_spots_value;

$fetchUsers = db_select('Match_contests_submit_info', 'm')
                          ->fields('m', array('contest_id'))
                         ->condition('m.contest_id', $nid);
$count = $fetchUsers->execute()->rowCount();
$contests['contests'][$key]['remaining_spots'] = $value->field_total_spots_value - $count;
$contests['contests'][$key]['total_winners'] = "Rs " . $value->field_total_winners_value;
}
}

return $contests;
}

function generating_checksum($mid, $orderid, $custid, $channelid, $amount, $website, $url, $industry, $merchantkey) {

 $paytmParams = array();
    $paytmParams["MID"] = $mid;
    $paytmParams["ORDER_ID"] = $orderid;
    $paytmParams["CUST_ID"] = $custid;
//    $paytmParams["MOBILE_NO"] = mobileNo;
//    $paytmParams["EMAIL"] = email;
    $paytmParams["CHANNEL_ID"] = $channelid;
    $paytmParams["TXN_AMOUNT"] = $amount;
    $paytmParams["WEBSITE"] = $website;
    $paytmParams["INDUSTRY_TYPE_ID"] = $industry;
    $paytmParams["CALLBACK_URL"] = $url;
  $paytmChecksum = getChecksumFromArray($paytmParams, $merchantkey);
    
    $result = array('CHECKSUMHASH' => $paytmChecksum, 'ORDER_ID' => $orderid, 'payt_STATUS' => 1);
    
    $object = (object) $result;
    
    return $object;
    
}

function encrypt_e($input, $ky) {
	$key   = html_entity_decode($ky);
	$iv = "@@@@&&&&####$$$$";
	$data = openssl_encrypt ( $input , "AES-128-CBC" , $key, 0, $iv );
	return $data;
}

function decrypt_e($crypt, $ky) {
	$key   = html_entity_decode($ky);
	$iv = "@@@@&&&&####$$$$";
	$data = openssl_decrypt ( $crypt , "AES-128-CBC" , $key, 0, $iv );
	return $data;
}

function generateSalt_e($length) {
	$random = "";
	srand((double) microtime() * 1000000);

	$data = "AbcDE123IJKLMN67QRSTUVWXYZ";
	$data .= "aBCdefghijklmn123opq45rs67tuv89wxyz";
	$data .= "0FGH45OP89";

	for ($i = 0; $i < $length; $i++) {
		$random .= substr($data, (rand() % (strlen($data))), 1);
	}

	return $random;
}

function checkString_e($value) {
	if ($value == 'null')
		$value = '';
	return $value;
}

function getChecksumFromArray($arrayList, $key, $sort=1) {
	if ($sort != 0) {
		ksort($arrayList);
	}
	$str = getArray2Str($arrayList);
	$salt = generateSalt_e(4);
	$finalString = $str . "|" . $salt;
	$hash = hash("sha256", $finalString);
	$hashString = $hash . $salt;
	$checksum = encrypt_e($hashString, $key);
	return $checksum;
}
function getChecksumFromString($str, $key) {
	
	$salt = generateSalt_e(4);
	$finalString = $str . "|" . $salt;
	$hash = hash("sha256", $finalString);
	$hashString = $hash . $salt;
	$checksum = encrypt_e($hashString, $key);
	return $checksum;
}

function verifychecksum_e($arrayList, $key, $checksumvalue) {
	$arrayList = removeCheckSumParam($arrayList);
	ksort($arrayList);
	
	$str = getArray2StrForVerify($arrayList);
	$paytm_hash = decrypt_e($checksumvalue, $key);
	$salt = substr($paytm_hash, -4);

	$finalString = $str . "|" . $salt;

	$website_hash = hash("sha256", $finalString);
	$website_hash .= $salt;

	$validFlag = "FALSE";
	if ($website_hash == $paytm_hash) {
		$validFlag = "TRUE";
	} else {
		$validFlag = "FALSE";
	}
	return $validFlag;
}

function verifychecksum_eFromStr($str, $key, $checksumvalue) {
	$paytm_hash = decrypt_e($checksumvalue, $key);
	$salt = substr($paytm_hash, -4);

	$finalString = $str . "|" . $salt;

	$website_hash = hash("sha256", $finalString);
	$website_hash .= $salt;

	$validFlag = "FALSE";
	if ($website_hash == $paytm_hash) {
		$validFlag = "TRUE";
	} else {
		$validFlag = "FALSE";
	}
	return $validFlag;
}

function getArray2Str($arrayList) {
	$findme   = 'REFUND';
	$findmepipe = '|';
	$paramStr = "";
	$flag = 1;	
	foreach ($arrayList as $key => $value) {
		$pos = strpos($value, $findme);
		$pospipe = strpos($value, $findmepipe);
		if ($pos !== false || $pospipe !== false) 
		{
			continue;
		}
		
		if ($flag) {
			$paramStr .= checkString_e($value);
			$flag = 0;
		} else {
			$paramStr .= "|" . checkString_e($value);
		}
	}
	return $paramStr;
}

function getArray2StrForVerify($arrayList) {
	$paramStr = "";
	$flag = 1;
	foreach ($arrayList as $key => $value) {
		if ($flag) {
			$paramStr .= checkString_e($value);
			$flag = 0;
		} else {
			$paramStr .= "|" . checkString_e($value);
		}
	}
	return $paramStr;
}

function redirect2PG($paramList, $key) {
	$hashString = getchecksumFromArray($paramList);
	$checksum = encrypt_e($hashString, $key);
}

function removeCheckSumParam($arrayList) {
	if (isset($arrayList["CHECKSUMHASH"])) {
		unset($arrayList["CHECKSUMHASH"]);
	}
	return $arrayList;
}

function getTxnStatus($requestParamList) {
	return callAPI(PAYTM_STATUS_QUERY_URL, $requestParamList);
}

function getTxnStatusNew($requestParamList) {
	return callNewAPI(PAYTM_STATUS_QUERY_NEW_URL, $requestParamList);
}

function initiateTxnRefund($requestParamList) {
	$CHECKSUM = getRefundChecksumFromArray($requestParamList,PAYTM_MERCHANT_KEY,0);
	$requestParamList["CHECKSUM"] = $CHECKSUM;
	return callAPI(PAYTM_REFUND_URL, $requestParamList);
}

function callAPI($apiURL, $requestParamList) {
	$jsonResponse = "";
	$responseParamList = array();
	$JsonData =json_encode($requestParamList);
	$postData = 'JsonData='.urlencode($JsonData);
	$ch = curl_init($apiURL);
	curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");                                                                     
	curl_setopt($ch, CURLOPT_POSTFIELDS, $postData);                                                                  
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); 
	curl_setopt ($ch, CURLOPT_SSL_VERIFYHOST, 0);
	curl_setopt ($ch, CURLOPT_SSL_VERIFYPEER, 0);
	curl_setopt($ch, CURLOPT_HTTPHEADER, array(                                                                         
	'Content-Type: application/json', 
	'Content-Length: ' . strlen($postData))                                                                       
	);  
	$jsonResponse = curl_exec($ch);   
	$responseParamList = json_decode($jsonResponse,true);
	return $responseParamList;
}

function callNewAPI($apiURL, $requestParamList) {
	$jsonResponse = "";
	$responseParamList = array();
	$JsonData =json_encode($requestParamList);
	$postData = 'JsonData='.urlencode($JsonData);
	$ch = curl_init($apiURL);
	curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");                                                                     
	curl_setopt($ch, CURLOPT_POSTFIELDS, $postData);                                                                  
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); 
	curl_setopt ($ch, CURLOPT_SSL_VERIFYHOST, 0);
	curl_setopt ($ch, CURLOPT_SSL_VERIFYPEER, 0);
	curl_setopt($ch, CURLOPT_HTTPHEADER, array(                                                                         
	'Content-Type: application/json', 
	'Content-Length: ' . strlen($postData))                                                                       
	);  
	$jsonResponse = curl_exec($ch);   
	$responseParamList = json_decode($jsonResponse,true);
	return $responseParamList;
}
function getRefundChecksumFromArray($arrayList, $key, $sort=1) {
	if ($sort != 0) {
		ksort($arrayList);
	}
	$str = getRefundArray2Str($arrayList);
	$salt = generateSalt_e(4);
	$finalString = $str . "|" . $salt;
	$hash = hash("sha256", $finalString);
	$hashString = $hash . $salt;
	$checksum = encrypt_e($hashString, $key);
	return $checksum;
}
function getRefundArray2Str($arrayList) {	
	$findmepipe = '|';
	$paramStr = "";
	$flag = 1;	
	foreach ($arrayList as $key => $value) {		
		$pospipe = strpos($value, $findmepipe);
		if ($pospipe !== false) 
		{
			continue;
		}
		
		if ($flag) {
			$paramStr .= checkString_e($value);
			$flag = 0;
		} else {
			$paramStr .= "|" . checkString_e($value);
		}
	}
	return $paramStr;
}
function callRefundAPI($refundApiURL, $requestParamList) {
	$jsonResponse = "";
	$responseParamList = array();
	$JsonData =json_encode($requestParamList);
	$postData = 'JsonData='.urlencode($JsonData);
	$ch = curl_init($apiURL);	
	curl_setopt ($ch, CURLOPT_SSL_VERIFYHOST, 0);
	curl_setopt ($ch, CURLOPT_SSL_VERIFYPEER, 0);
	curl_setopt($ch, CURLOPT_URL, $refundApiURL);
	curl_setopt($ch, CURLOPT_POST, true);
	curl_setopt($ch, CURLOPT_POSTFIELDS, $postData);  
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); 
	$headers = array();
	$headers[] = 'Content-Type: application/json';
	curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);  
	$jsonResponse = curl_exec($ch);   
	$responseParamList = json_decode($jsonResponse,true);
	return $responseParamList;
}


function getQuizContestsSubmissions($answer_list) {

foreach($answer_list as $key =>  $value) {
//foreach($value as $keys => $ans) {

$uid = $value['uid'];
$contestid = $value['contestid'];
$quizid = $value['quizid'];

$nid = db_insert('betting')
      ->fields(array(
        'uid' => $value['uid'],
        'nid' => $value['quizid'],
        'form_id' => $value['questionId'],
        'select_value' => $value['selectedAnswer'],
         'contest_id' => $value['contestid'],
          'timetaken' => $value['timeTaken'],
      ))
      ->execute();

//}
}
$fee = 0;
$entryfee = db_select('field_data_field_entry_fee', 'fee')
           ->fields('fee', array('field_entry_fee_value'))
           ->condition('fee.entity_id', $contestid);
$fee = $entryfee->execute()->fetchField();
$nid = db_insert('Match_contests_submit_info')
      ->fields(array(
        'contest_id' => $contestid,
        'uid' => $uid,
        'match_id' => $quizid,
        'entry_amount' => $fee,
        'timestamp' => REQUEST_TIME,
      ))
      ->execute();
      

// Update Referal User Cashbonus if the current user joined the contest for the first time.

$user = user_load($uid);
if (isset($user->field_first_contest_joined_or_no['und'][0]['value']) && $user->field_first_contest_joined_or_no['und'][0]['value'] == 0) {


$cash_bonus_of_referral_user = ceil($fee/10);

// get referal user current cash bonus
$referal_user_uid = isset($user->field_add_referal_user_id['und'][0]['target_id']) ? $user->field_add_referal_user_id['und'][0]['target_id'] : NULL;

if (!empty($referal_user_uid)) {
  $referal_user = user_load($referal_user_uid);
  
  //get current referal_user_cashbonus and add the new one
  
  $current_cashbonus = $referal_user->field_cash_bonus['und'][0]['value'];
  
  $total_cashbonus = (int)$current_cashbonus + $cash_bonus_of_referral_user;
  
  $referal_user->field_cash_bonus['und'][0]['value'] = $total_cashbonus;
  user_save($referal_user);
  
  $user->field_first_contest_joined_or_no['und'][0]['value'] = 1;
  user_save($user);
}
}

      

$status = get_wining_amount_for_quiz_and_contest($quizid, $uid, $contestid);
return $status;


}

function get_wining_amount_for_quiz_and_contest($quizid, $uid, $contest_id) {
   
   // check correct answers
    //retrieve correct answers for the Quiz

   
$query = db_select('field_data_field_correct_options', 'correct')
               ->fields('correct', array('entity_id', 'field_correct_options_value'))
               ->condition('quiz.field_quiz_id_target_id', $quizid)
               ->orderby('correct.entity_id');
           $query->join('field_data_field_quiz_id', 'quiz', "quiz.entity_id = correct.entity_id");
           $results = $query->execute()->fetchAllAssoc('entity_id');

           $correct_answers = array();
	foreach($results as $key => $value) {
		$correct_answers[$key] = $value->field_correct_options_value;
	}
$query = db_select('betting', 'actual')
           ->fields('actual', array('form_id', 'select_value'))
           ->condition('actual.nid', $quizid)
           ->condition('actual.uid', $uid)
           ->condition('actual.contest_id', $contest_id);
        $filled_answers =    $query->execute()->fetchAllAssoc('form_id');
$user_answers = array();
	foreach($filled_answers as $key => $value) {
		$user_answers[$key] = $value->select_value;
	}
$correct_answers_in_quiz = array_intersect($user_answers, $correct_answers);

if (empty($correct_answers_in_quiz)) {
 $status = array('error' => 'You Answered all Wrong Questions for this Quiz. You Lose');
 
 return $status;
}

else {

$query = db_select('betting', 'actual')
           ->fields('actual', array('form_id', 'timetaken'))
           ->condition('actual.nid', $quizid)
           ->condition('actual.uid', $uid)
           ->condition('actual.contest_id', $contest_id);
$time_of_answers =$query->execute()->fetchAllAssoc('form_id');
$time_answers = array();
if (!empty($time_of_answers)) {
  foreach($time_of_answers as $key => $value) {
    $time_answers[$key] = $value->timetaken;
  }

  $a3 = array_intersect_key($time_answers, $correct_answers_in_quiz);

  $sum = array_sum($a3);

 $points = 0;
  $points = 100000 - $sum;


        // Store points in a Table

       $nid = db_insert('quiz_points')
      ->fields(array(
        'uid' => $uid,
        'nid' => $quizid,
       'points' => $points,
       'contest_id' => $contest_id,
      ))
      ->execute();

      $message = array('points' => "You have " . $points);
     return $message;

}  
}

$message = array('points' => "You scored 0 Points");

}



function update_quiz_winning_amount($contest_id) {
$query  = db_select('node', 'n')
               ->fields('d', array('field_ranking_division_value'))
               ->condition('n.status', 1)
               ->condition('n.nid', $contest_id);

$query->join('field_data_field_ranking_division', 'd', "d.entity_id = n.nid");
$contests = $query->execute()->fetchCol();
$data = array();

if (!empty($contests)) {
foreach($contests as $key => $value) {
$query = db_select('field_data_field_amount_distribution', 'a')
              ->fields('a', array('field_amount_distribution_value'))
             ->fields('r', array('field_rank_number_value'))
  ->condition('a.entity_id', $value);

$query->join('field_data_field_rank_number', 'r', "r.entity_id = a.entity_id");
$results = $query->execute()->fetchAll();
foreach($results as $rank => $amount) {

$explode_rank = explode('-', $amount->field_rank_number_value);
foreach($explode_rank as $key => $value) {
$data[$value]=  $amount->field_amount_distribution_value;
}

}
}
}
$data = array_values($data);
$total_users_winning_amount_updated = count($data);
$getUsers = db_select('quiz_points', 'm')
               ->fields('m', array('uid')) 
              ->condition('m.contest_id', $contest_id);
$query->orderby('m.points', 'DESC');

$all_uids = $getUsers->execute()->fetchCol();
$count_users = count($all_uids);

for($i = 0; $i < $count_users; $i++) {
 if ($i < $total_users_winning_amount_updated) {

     $num_updated = db_update('Match_contests_submit_info')
    ->fields(array(
      'winning_amount' => $data[$i],
    ))
    ->condition('contest_id', $contest_id, '=')
    ->condition('uid', $all_uids[$i] , '=')
    ->execute();
}
else {
 $num_updated = db_update('Match_contests_submit_info')
    ->fields(array(
      'winning_amount' => 0,
    ))
    ->condition('contest_id', $contest_id, '=')
    ->condition('uid', $all_uids[$i] , '=')
    ->execute();
}

$user_info = user_load($all_uids[$i]);

if (isset($user_info->field_total_win_amount['und'][0]['value'])) {
  $amount = $user_info->field_total_win_amount['und'][0]['value'];
  
  //update wining amount
  if (isset($data[$i]) && !empty($data[$i])) {
    $amount = $amount + (int)$data[$i];
  }
  
  $user_info->field_total_win_amount['und'][0]['value'] = $amount;
  user_save($user_info);
}


}

$message = array("status" => "Updated Amount of All users participated in Contests");

return $message;

}



function setMatchSubmissions($answer_list) {
foreach($answer_list as $key=>$value) {

//foreach($value as $keys => $ans) {
$nid = db_insert('betting')
      ->fields(array(
        'uid' => $value['uid'],
        'nid' => $value['matchid'],
        'form_id' => $value['quesid'],
        'select_value' => $value['selected_option'],
 'contest_id' => $value['contestid'],
'timetaken' => 0,
      ))
      ->execute();

//}
}

$message = array('status' => 'Your responses are recorded, JOIN THE CONTEST');
return $message;
}





function joinMatchContest($contest_id, $match_id, $uid) {


$entryfee = db_select('field_data_field_entry_fee', 'fee')
           ->fields('fee', array('field_entry_fee_value'))
           ->condition('fee.entity_id', $contest_id);
$fee = $entryfee->execute()->fetchField();

if (empty($fee)) {
$fee = 0;
}

$nid = db_insert('Match_contests_submit_info')
      ->fields(array(
        'contest_id' => $contest_id,
        'uid' => $uid,
        'match_id' => $match_id,
        'entry_amount' => $fee,
        'timestamp' => REQUEST_TIME,
      ))
      ->execute();
    
      $num_updated = db_update('betting')
    ->fields(array(
      'contest_id' => $contest_id,
    ))
    ->condition('nid', $match_id, '=')
    ->condition('uid', $uid, '=')
    ->execute();
    
    
    // Update Referal User Cashbonus if the current user joined the contest for the first time.

$user = user_load($uid);
if (isset($user->field_first_contest_joined_or_no['und'][0]['value']) && $user->field_first_contest_joined_or_no['und'][0]['value'] == 0) {


$cash_bonus_of_referral_user = ceil($fee/10);

// get referal user current cash bonus
$referal_user_uid = isset($user->field_add_referal_user_id['und'][0]['target_id']) ? $user->field_add_referal_user_id['und'][0]['target_id'] : NULL;

if (!empty($referal_user_uid)) {
  $referal_user = user_load($referal_user_uid);
  
  //get current referal_user_cashbonus and add the new one
  
  $current_cashbonus = $referal_user->field_cash_bonus['und'][0]['value'];
  
  $total_cashbonus = (int)$current_cashbonus + $cash_bonus_of_referral_user;
  
  $referal_user->field_cash_bonus['und'][0]['value'] = $total_cashbonus;
  user_save($referal_user);
  
  $user->field_first_contest_joined_or_no['und'][0]['value'] = 1;
  user_save($user);
}
}

      $message = array('status' => 'Thanks for Joining the contest, You will get to know about the results');
      
      return $message;
}



function getResult($uid) {

  $match_id =  variable_get('current_match_id', '');
  $final_answers = array(
        1 => variable_get('match_toss', ''),
        2 => variable_get('match_powerplay_runs', ''),
        3 => variable_get('highest_wicket_fall', ''),
        4 =>  variable_get('highest_fours_match', ''),
        5 => variable_get('highest_six_match', ''),
        6 => variable_get('extra_runs_given', ''),
        7 => variable_get('top_batsman_match', ''),
        8 => variable_get('top_bowler_match', ''),
        9 => variable_get('highest_partnership', ''),
        10 => variable_get('highest_catch_without_keeper', ''),
        11 => variable_get('highest_dismissal_keeper', ''),
        12 => variable_get('player_score_50', ''),
        13 =>  variable_get('score_50+', ''),
        14 => variable_get('highest_poweplay_4s', ''),
        15 => variable_get('top_batsman_team_a', ''),
        16 => variable_get('top_batsman_team_b', ''),
        17 => variable_get('top_bowler_team_a', ''),
        18 => variable_get('top_bowler_team_b', ''),
        19 => variable_get('man_of_the_match', ''),
        20 => variable_get('match_winner', ''),
      );

// Get All contests id related to that match.

$query =  db_select('node', 'n')
         ->fields('n', array('nid'))
          ->condition('n.status', 1)
         ->condition('n.type', 'match_contests')
       ->condition('m.field_match_id_target_id', $match_id);

$query->join('field_data_field_match_id', 'm', "m.entity_id = n.nid");

$all_cids = $query->execute()->fetchCol();

if (!empty($all_cids)) {
 foreach($all_cids as $cid) {

$getUsers = db_select('Match_contests_submit_info', 'm')
               ->fields('m', array('uid')) 
              ->condition('m.contest_id', $cid)
              ->condition('m.match_id', $match_id);

$all_uids = $getUsers->execute()->fetchCol();
foreach($all_uids as $uid) {
$query = db_select('betting', 'bet')
            ->fields('bet', array('form_id', 'select_value'))
            ->condition('bet.nid' , $match_id)
 ->condition('bet.uid' , $uid)
            ->orderby('bet.form_id');
          
   $results = $query->execute()->fetchAllAssoc('form_id');
 $user_answers = array();
  foreach($results as $key => $value) {
    $user_answers[$key] = $value->select_value;
  }
  $correct_answers = array_intersect($final_answers, $user_answers);
  $correct_answers_count = count($correct_answers);

  $entryfee = db_select('field_data_field_entry_fee', 'fee')
           ->fields('fee', array('field_entry_fee_value'))
           ->condition('fee.entity_id', $cid);
$fee = $entryfee->execute()->fetchField();

if (empty($fee)) {
  $fee = 0;
}

switch ($correct_answers_count) {
  case 20:
      $wining_amount = 10 * $fee;
    break;

  case 19:
      $wining_amount = 5 * $fee;
    break;
    
  case 18:
      $wining_amount = 4 * $fee;
    break;
    
   case 17:
      $wining_amount = 3 * $fee;
    break;     
  
  case 16:
      $wining_amount = 2 * $fee;
    break;
  default:
    $wining_amount = 0;
    break;
}

$num_updated = db_update('Match_contests_submit_info')
    ->fields(array(
      'winning_amount' => $wining_amount ,
    ))
    ->condition('match_id', $match_id, '=')
    ->condition('uid', $uid, '=')
    ->condition('contest_id', $cid, '=')
    ->execute();
    
    $user_info = user_load($uid);
if (isset($user_info->field_total_win_amount['und'][0]['value'])) {
  $amount = $user_info->field_total_win_amount['und'][0]['value'];
    $amount = $amount + $wining_amount;
  $user_info->field_total_win_amount['und'][0]['value'] = $amount;
  user_save($user_info);
}
    
    
  }
}

}

$message = array("status" => "Result calculated of the Match");
return $message;

}


function getuserData($uid) {
  $user_info = user_load($uid);
$user_data = array();

$user_data['name'] = $user_info->name;
$user_data['mail'] = $user_info->mail;
$user_data['mobile_number'] = isset($user_info->field_mobile_number['und'][0]['value']) ? $user_info->field_mobile_number['und'][0]['value'] : NULL ;
$user_data['city'] = isset($user_info->field_city['und'][0]['value']) ? $user_info->field_city['und'][0]['value'] : NULL;
$user_data['state'] = isset($user_info->field_state['und'][0]['value'])? $user_info->field_state['und'][0]['value'] : NULL;
$user_data['pin'] = isset($user_info->field_pin_code['und'][0]['value']) ?$user_info->field_pin_code['und'][0]['value'] : NULL ;


return $user_data;

}



function getWinnersQuizContest($quiz_id, $contest_id) {
    $query = db_select('Match_contests_submit_info', 'm')
             ->fields('m', array('entry_amount', 'winning_amount'))
             ->fields('u', array('name'))
             ->condition('m.match_id', $quiz_id)
             ->condition('m.contest_id', $contest_id)
             ->condition('m.winning_amount', 0, '>');
      $query->join('users', 'u', "u.uid = m.uid");
     $query->orderby('winning_amount', 'DESC');
     $results =   $query->execute()->fetchAll();      
$winning_data = array();
if (!empty($results)) {
foreach($results as $key => $value) {
$winning_data[$key]['name'] = $value->name;
$winning_data[$key]['entry_amount'] = $value->entry_amount;
$winning_data[$key]['winning_amount'] = $value->winning_amount;
}
}
else {
  $winning_data = array('status' => 'No Winners Yet');
}

return $winning_data;
    
}


function getUserContestData($uid) {
    $query = db_select('Match_contests_submit_info', 'm')
             ->fields('m', array('match_id', 'contest_id', 'entry_amount', 'winning_amount', 'timestamp'))
             ->condition('m.uid', $uid);
     $query->orderby('winning_amount', 'DESC');
     $results =   $query->execute()->fetchAll();      
$winning_data = array();
if (!empty($results)) {
foreach($results as $key => $value) {
$winning_data[$key]['entry_amount'] = $value->entry_amount;
$winning_data[$key]['winning_amount'] = $value->winning_amount;
$winning_data[$key]['date'] = date("Y-m-d", $value->timestamp);
$mid = $value->match_id;
$match_name = db_select('node', 'n')
->fields('n', array('title'))
->condition('n.nid', $mid);
$match_title = $match_name->execute()->fetchField();

$cid = $value->contest_id;
$contest_name = db_select('node', 'n')
->fields('n', array('title'))
->condition('n.nid', $cid);
$contest_title = $contest_name->execute()->fetchField();

$winning_data[$key]['match_name'] = $match_title;
$winning_data[$key]['contest_name'] = $contest_title;
}
}
else {
  $winning_data = array('status' => 'No Results Found');
}

$object = new stdClass();
$object->data = $winning_data;

return $object;
    
}

function getMatchContestWinnerData($match_id, $contest_id) {
    $query = db_select('Match_contests_submit_info', 'm')
             ->fields('m', array('entry_amount', 'winning_amount'))
             ->fields('u', array('name'))
             ->condition('m.match_id', $match_id)
             ->condition('m.contest_id', $contest_id);
      $query->join('users', 'u', "u.uid = m.uid");
     $query->orderby('winning_amount', 'DESC');
     $results =   $query->execute()->fetchAll();      
$winning_data = array();
if (!empty($results)) {
foreach($results as $key => $value) {
$winning_data[$key]['name'] = $value->name;
$winning_data[$key]['entry_amount'] = $value->entry_amount;
$winning_data[$key]['winning_amount'] = $value->winning_amount;
}

$object = new stdClass();
$object->data = $winning_data;
}
else {
  $data = array('data' => array(), 'status' => 'No results found');
  $object = new stdClass();
  $object = $data;
}



return $object;
}


function getQuizWinningCriteria($contest_id) {
$query  = db_select('node', 'n')
               ->fields('d', array('field_ranking_division_value'))
               ->condition('n.status', 1)
               ->condition('n.nid', $contest_id);

$query->join('field_data_field_ranking_division', 'd', "d.entity_id = n.nid");
$contests = $query->execute()->fetchCol();
$data = array();

if (!empty($contests)) {
foreach($contests as $key => $value) {
$query = db_select('field_data_field_amount_distribution', 'a')
              ->fields('a', array('field_amount_distribution_value'))
             ->fields('r', array('field_rank_number_value'))
  ->condition('a.entity_id', $value);

$query->join('field_data_field_rank_number', 'r', "r.entity_id = a.entity_id");
$results = $query->execute()->fetchAll();
foreach($results as $rank => $amount) {
$data[$key]['rank'] = $amount->field_rank_number_value;
$data[$key]['amount'] =  $amount->field_amount_distribution_value;
}
}

$object = new stdClass();
$object->data = $data;
}

else {
   $data = array('data' => array(), 'status' => 'No results found');
   $object = new stdClass();
$object = $data;
}




return $object;

}


function getMatchWinCriteria($contest_id) {
$query  = db_select('node', 'n')
               ->fields('d', array('field_entry_fee_value'))
               ->condition('n.status', 1)
               ->condition('n.nid', $contest_id);

$query->join('field_data_field_entry_fee', 'd', "d.entity_id = n.nid");
$entry_fee = $query->execute()->fetchField();
if (!empty($entry_fee)) {
$data = array("status" => "For Entry Rs. " . $entry_fee . "\n If you correct 20/20  you get " . 10 * $entry_fee     . "\n         19/20                                  " . 5 * $entry_fee . "\n      18/20                                       " . 4 * $entry_fee . "\n       17/20                                      " . 3 * $entry_fee . "\n       16/20                                      " . 2 * $entry_fee);
$object = new stdClass();
$object = $data;

}
else {
 $data = array('status' => 'No Entry fee is given');
 $object = new stdClass();
 $object = $data;
}

return $object;

}



//get users who joined referal with pass uid
function getreferalUsers($uid) {

$query = db_select('users', 'u')
           ->fields('r', array('field_referal_code_value'))
           ->condition('u.uid', $uid);

$query->join('field_data_field_referal_code', 'r', "u.uid = r.entity_id");
$code = $query->execute()->fetchField();

// Find users who has the above code in their referal
$query = db_select('users', 'u')
           ->fields('u', array('name'))
           ->fields('c', array('field_referal_code_value'))
           ->condition('r.field_add_user_referal_code_value', $code);

$query->join('field_data_field_add_user_referal_code', 'r', "u.uid = r.entity_id");
$query->join('field_data_field_referal_code', 'c', "u.uid = c.entity_id");

$results = $query->execute()->fetchAll();
$user_joined_referal = array();
if (!empty($results)) {
foreach($results as $key => $value) {
$user_joined_referal[$key]['name'] = $value->name;
$user_joined_referal[$key]['referal_code'] = $value->field_referal_code_value;
}
}
if (empty($user_joined_referal)) {
$data = array('status' => 'No results found');

return $data;

}

return $user_joined_referal;

}

function get_cash_bonus_user($uid) {
$query = db_select('users', 'u')
           ->fields('r', array('field_cash_bonus_value'))
           ->condition('u.uid', $uid);

$query->join('field_data_field_cash_bonus', 'r', "u.uid = r.entity_id");
$cash_bonus = $query->execute()->fetchField();

if (empty($cash_bonus)) {
   $cash_bonus = 0;
}

$message = array('status' => $cash_bonus);

return $message;

}

function get_win_amount_user($uid) {
/*
$query =  db_select('Match_contests_submit_info', 'n')
         ->fields('n', array('winning_amount'))
         ->condition('n.uid', $uid);
$win_amount = $query->execute()->fetchCol();

$win_amount = array_sum($win_amount);
*/

$user_info = user_load($uid);
$win_amount = isset($user_info->field_total_win_amount['und'][0]['value']) ? $user_info->field_total_win_amount['und'][0]['value']: 0;

$message = array('status' => $win_amount);

return $message;

}


function getShowEntryData($contest_id, $match_id, $uid) {
$query = db_select('betting', 'b')
           ->fields('b', array('select_value'))
           ->condition('uid', $uid)
          ->condition('nid', $match_id)
            ->orderby('form_id')
           ->range(0,20)
           ->condition('contest_id', $contest_id);
          
         
$results = $query->execute()->fetchCol();
$categories = array(
          0 => 'MATCH TOSS',
          1 => 'HIGHEST POWERPLAY RUNS',
          2 => 'HIGHEST WICKET FALL',
          3 => 'HIGHEST 4S IN MATCH',
          4 => 'HIGHEST 6S IN MATCH',
          5 => 'EXTRA RUNS GIVEN',
          6 => 'TOP RUN SCORER',
          7 => 'TOP BOWLER IN MATCH',
          8 => 'HIGHEST OPENING PARTNERSHIP',
          9 => 'HIGHEST CATCH OUT',
          10 => 'HIGHEST DISMISSAL BY KEEPER',
          11 => 'PLAYER SCORE 50',
          12 => '50+ SCORE IN MATCH',
          13 => 'HIGHEST POWERPLAY BOUNDARIES',
          14 => 'TOP BATSMAN TEAM A',
          15 => 'TOP BATSMAN TEAM B',
          16 => 'TOP BOWLER TEAM A',
          17 => 'TOP BOWLER TEAM B',
          18 => 'MAN OF THE MATCH',
          19 => 'MATCH WINNER',
);
$final = array();
for ($i = 0; $i < 20; $i++) {
  $final[$i]['ques'] = $categories[$i];
 $final[$i]['ans'] = $results[$i];
}

$object = new stdClass();
$object->data = $final;
return $object;

}


function check_entry_exists($match_id, $uid) {
$query = db_select('betting', 'b')
           ->fields('b', array('select_value'))
           ->condition('uid', $uid)
          ->condition('nid', $match_id)
            ->orderby('form_id')
           ->range(0,20)
           ->condition('contest_id', 0);
          
         
$results = $query->execute()->rowCount();

if ($results == 0) {
   $message = array('status' => 'Create Entry');
}
else {

$message = array('status' => 'Show Entry');

}

return $message;
}



