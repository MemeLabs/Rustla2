// @flow

import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import idx from 'idx';
import type { BrowserHistory } from 'history';
import type { Match } from 'react-router-dom';

import { fetchPoll, submitPollVote } from '../actions';
import MainLayout from './MainLayout';
import Checkbox from './Checkbox';
import type { State } from '../redux/types';

import type { Poll } from '../records/polls';

type SubmitPollVote = ({ options: Array<string> }) => void;

const SingleSelectInput = ({ value }: { value: string }) => (
  <div className='form-group col-sm-12'>
    <label>
      <div className='poll-vote-input'>
        <input
          className='form-control form-radio-input'
          name='options'
          value={value}
          type='radio'
        />
      </div>
      <span className='poll-vote-label'>{value}</span>
    </label>
  </div>
);

const MultiSelectInput = ({ value }: { value: string }) => (
  <div className='form-group col-sm-12'>
    <label className='poll-vote-row'>
      <div className='poll-vote-input'>
        <Checkbox
          className='form-control form-checkbox-input'
          name={`options`}
          value={value}
        />
      </div>
      <span className='poll-vote-label'>{value}</span>
    </label>
  </div>
);

type PollFormProps = {
  onSubmit: SubmitPollVote;
  poll: Poll;
};

class PollForm extends React.Component<PollFormProps> {
  handleSubmit = (event) => {
    event.preventDefault();

    const options = Array.from(event.target.elements)
      .filter(el => el.name === 'options' && el.checked)
      .map(el => el.value);

    if (options.length === 0) {
      return;
    }

    this.props.onSubmit({options});
  }

  render() {
    const {
      subject,
      options,
      multi_vote: multiVote,
    } = this.props.poll;

    const Input = multiVote ? MultiSelectInput : SingleSelectInput;
    const optionInputs = Object.entries(options).map(([value], i) => (
      <Input key={i} value={value} />
    ));

    return (
      <form onSubmit={this.handleSubmit}>
        <h1 className='col-sm-12 poll-title'>{subject}</h1>
        {optionInputs}
        <div className='form-group'>
          <div className='col-sm-12'>
            <button
              type='submit'
              className='btn btn-primary'
              disabled={false}
            >
              Submit
            </button>
          </div>
        </div>
      </form>
    );
  }
}

type PollVoteOwnProps = {|
  +history: BrowserHistory,
  +match: Match
|};
type PollVoteProps = {|
  ...PollVoteOwnProps,
  +fetchPoll: () => void,
  +poll: ?Poll,
  +submitPollVote: SubmitPollVote
|};

const PollVote = ({ poll, history, submitPollVote }: PollVoteProps) => {
  const pollForm = poll && poll.loaded
    ? <PollForm onSubmit={submitPollVote} poll={poll} />
    : <h4 className='poll-loading'>Loading...</h4>;

  return (
    <MainLayout history={history}>
      <div className='container'>
        {pollForm}
      </div>
    </MainLayout>
  );
};

function mapStateToProps(
  state: State,
  ownProps: PollVoteOwnProps
): $Shape<PollVoteProps> {
  const id = ownProps.match.params.poll;
  if (!id) {
    return {};
  }

  const poll = idx(state, _ => _.polls[id]);
  if (!poll) {
    return {};
  }

  return { poll };
}

function mapDispatchToProps(
  dispatch,
  props: PollVoteOwnProps
): $Shape<PollVoteProps> {
  return {
    submitPollVote(options) {
      return dispatch(submitPollVote(
        props.match.params.poll,
        options,
        props.history,
      ));
    },
    fetchPoll(id) {
      return dispatch(fetchPoll(id));
    }
  };
}

export default compose(
  connect<PollVoteProps, PollVoteOwnProps, _, _, _, _>(
    mapStateToProps,
    mapDispatchToProps
  ),
  lifecycle({
    componentDidMount() {
      this.props.fetchPoll(this.props.id);
    },
  }),
)(PollVote);
